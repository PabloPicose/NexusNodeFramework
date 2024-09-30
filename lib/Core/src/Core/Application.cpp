//
// Created by pablo on 5/28/24.
//

#include "Application.h"
#include <iostream>
#include "Core/Node.h"
#include "Core/NodePtr.h"

namespace IRC {
    Application* Application::m_instance = nullptr;

    Application::Application() : m_threadId(std::this_thread::get_id()) {
        if (m_instance) {
            throw std::runtime_error("Only one instance of Application is allowed");
        } else {
            m_instance = this;
        }
    }

    Application::~Application() {
        for (Node* node: m_rootNodes) {
            std::cout << "Deleting root node" << std::endl;
            delete node;
        }
        m_rootNodes.clear();

        m_instance = nullptr;
    }


    std::string Application::getFullExecutablePath() const {
        // linux
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        return (count != -1) ? std::string(result, count) : std::string();
    }

    Application* Application::instance() { return m_instance; }

    std::thread::id Application::threadId() { return std::this_thread::get_id(); }

    void Application::loopOnce() {
        // controlls the nodes to delete
        for (Node* node: m_rootNodesToDelete) {
            NodePtr nodePtr(node);
            if (nodePtr) {
                delete node;
            }
        }
        m_rootNodesToDelete.clear();

        for (Node* node: m_rootNodes) {
            node->run();
        }
    }

    int Application::run() {
        while (!m_quit && m_rootNodes.size() > 0) {
            loopOnce();
        }
        return 0;
    }

    void Application::quit() { m_quit = true; }

    std::size_t Application::getRootNodesCount() const { return m_rootNodes.size(); }

    Node* Application::getRootNode(const std::size_t index) const {
        if (index < m_rootNodes.size()) {
            return m_rootNodes[index];
        }
        return nullptr;
    }

    bool Application::isNodeAlive(Node* node) const {
        if (m_aliveNodes.contains(node)) {
            return true;
        }
        return false;
    }

    bool Application::isNodeMarkedToDelete(Node* node) const {
        if (m_aliveNodes.contains(node)) {
            return !m_aliveNodes.at(node);
        }
        return true;
    }

    size_t Application::getRootNodesToDeleteCount() const { return m_rootNodesToDelete.size(); }

    size_t Application::getAliveNodesCount() const { return m_aliveNodes.size(); }

    size_t Application::getAliveNodesToDeleteCount() const {
        size_t count = 0;
        for (const auto& [node, alive]: m_aliveNodes) {
            if (!alive) {
                count++;
            }
        }
        return count;
    }

    void Application::pushRootNode(Node* node) {
        if (threadId() == m_threadId) {
            m_rootNodes.push_back(node);
        } else {
            throw std::runtime_error("Node can only be added from the main thread");
        }
    }

    void Application::pushRootNodeDeleteLater(Node* node) {
        if (threadId() == m_threadId) {
            // if the node is not in the m_rootNodes is an error.
            if (std::ranges::find(m_rootNodes, node) == m_rootNodes.end()) {
                throw std::runtime_error("Node not found in the root nodes");
            }
            m_rootNodesToDelete.push_back(node);
        } else {
            throw std::runtime_error("Node can only be added from the main thread");
        }
    }

    void Application::removeRootNode(Node* node) {
        for (auto it = m_rootNodes.begin(); it != m_rootNodes.end(); ++it) {
            if (*it == node) {
                m_rootNodes.erase(it);
                return;
            }
        }
        std::cout << "Node not found" << std::endl;
    }

    void Application::registerAliveNode(Node* node) { m_aliveNodes[node] = true; }

    void Application::markToDelete(Node* node) {
        if (!m_aliveNodes.contains(node)) {
            throw std::runtime_error("Node not found in the alive nodes");
        }
        m_aliveNodes[node] = false;
    }

    void Application::unregisterAliveNode(Node* node) {
        if (!m_aliveNodes.contains(node)) {
            throw std::runtime_error("Node not found in the alive nodes");
        }
        m_aliveNodes.erase(node);
    }

} // namespace IRC
