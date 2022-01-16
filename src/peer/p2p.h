#ifndef _P2P_H_
#define _P2P_H_

//
// Copyright (c) 2003-2021 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/streambuf.hpp>

using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::redirect_error;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;

class socket_handler
{
    tcp::socket socket;

public:
    socket_handler(tcp::socket socket) : socket(std::move(socket)) {}
    ~socket_handler()
    {
        if (!socket.is_open())
            socket.close();
    }
    tcp::socket &get_socket()
    {
        return socket;
    }
};
//----------------------------------------------------------------------

class chat_participant
{
public:
    virtual ~chat_participant() {}
    virtual void deliver(const std::string &msg) = 0;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;

//----------------------------------------------------------------------

class chat_room
{
public:
    void join(chat_participant_ptr participant)
    {
        participants_.insert(participant);
    }

    void leave(chat_participant_ptr participant)
    {
        participants_.erase(participant);
    }

    void deliver(const std::string &msg)
    {
        recent_msgs_.push_back(msg);
        while (recent_msgs_.size() > max_recent_msgs)
            recent_msgs_.pop_front();

        for (auto participant : participants_)
            participant->deliver(msg);
    }

private:
    std::set<chat_participant_ptr> participants_;
    enum
    {
        max_recent_msgs = 100
    };
    std::deque<std::string> recent_msgs_;
};

//----------------------------------------------------------------------

class chat_session
    : public chat_participant,
      public std::enable_shared_from_this<chat_session>
{
    std::string name;

public:
    chat_session(tcp::socket socket, chat_room &room)
        : socket_(std::make_shared<socket_handler>(std::move(socket))),
          room_(room)
    {
    }

    chat_session(std::shared_ptr<socket_handler> socket_hdl, chat_room &room) : socket_(socket_hdl),
                                                                                room_(room)
    {
    }
    void start()
    {

        room_.join(shared_from_this());

        std::thread read_thread(&chat_session::reader, this);
        read_thread.detach();
        std::thread write_thread(&chat_session::writer, this);
        write_thread.detach();
    }

    void deliver(const std::string &msg)
    {
        write_msgs_.push_back(msg);
        std::lock_guard<std::mutex> lock(session_mutex);
        session_cond.notify_one();
    }

    void parse_name(std::string msg)
    {

        std::string::size_type pos = msg.find(':');
        if (pos != std::string::npos)
        {
            name = msg.substr(0, pos);
        }
        else
        {
            name = msg;
        }
    }

private:
    void *reader()
    {

        try
        {
            for (std::string read_msg;;)
            {
                if (!socket_->get_socket().is_open())
                {
                    stop();
                    break;
                }

                std::size_t n = boost::asio::read_until(socket_->get_socket(),
                                                        boost::asio::dynamic_buffer(read_msg, 1024), "\n");
                std::cout << read_msg << std::endl;
                if (name.empty())
                {
                    parse_name(read_msg);
                }
                read_msg.erase(0, n);
            }
        }
        catch (std::exception &)
        {
            // std::cout << "read exception" << std::endl;
            std::cout << name << " has left the chat!" << std::endl;
            stop();
        }
    }

    void *writer()
    {
        try
        {
            while (socket_->get_socket().is_open())
            {

                std::unique_lock<std::mutex> lock(session_mutex);
                session_cond.wait_for(lock,
                                      std::chrono::seconds(1000),
                                      [&]()
                                      { return !write_msgs_.empty(); });

                boost::asio::write(socket_->get_socket(),
                                   boost::asio::buffer(write_msgs_.front()));
                write_msgs_.pop_front();
            }
        }
        catch (std::exception &)
        {
            std::cout << "writer exception" << std::endl;
            stop();
        }
    }

    void stop()
    {
        room_.leave(shared_from_this());
    }

    std::shared_ptr<socket_handler> socket_;
    chat_room &room_;
    std::deque<std::string> write_msgs_;
    std::mutex session_mutex;
    std::condition_variable session_cond;
};

//----------------------------------------------------------------------

extern chat_room room;
awaitable<void> listener(tcp::acceptor acceptor);

void *p2p_server_init(unsigned short port);
bool p2p_connect(std::string name, std::string peer, unsigned short port);
bool broadcast(std::string name, std::string msg);

#endif