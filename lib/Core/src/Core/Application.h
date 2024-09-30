//
// Created by pablo on 5/28/24.
//

#ifndef APPLICATION_H
#define APPLICATION_H

// Note: This header cannot contain the include of Node.h because the NodePtr template class include the Node and the
// Application header

#include <unordered_map>

namespace IRC {
    class Node;

    class Application {
    public:
        Application();

        ~Application();

        std::string getFullExecutablePath() const;

        static Application* instance();

        static std::thread::id threadId();

        void loopOnce();

        int run();

        void quit();

        std::size_t getRootNodesCount() const;

        Node* getRootNode(std::size_t index) const;

        /**
         * @brief Gets if the node pointer is still valid. This not checks if the node is about to delete.
         * @param node The node pointer to check
         * @return True if the node memory is accessible, false otherwise
         */
        bool isNodeAlive(Node* node) const;

        /**
         * @brief Gets if the node pointer is marked to be deleted. If the node is marked to be deleted OR
         * the node is not memory accessible, this function returns true.
         * @details It is important to check if the node is alive (memory accesible) before calling this function.
         * @param node The node pointer to check
         * @return True if the node is marked to be deleted OR the node is not memory accessible, false otherwise.
         */
        bool isNodeMarkedToDelete(Node* node) const;

        size_t getRootNodesToDeleteCount() const;

        size_t getAliveNodesCount() const;

        size_t getAliveNodesToDeleteCount() const;

    private:
        void pushRootNode(Node* node);

        void pushRootNodeDeleteLater(Node* node);

        void removeRootNode(Node* node);

        //! This function must be called in the constructor of the Node class.
        //! Creates a key in the map with the node and the boolean value to true
        void registerAliveNode(Node* node);

        //! This function marks to false the boolean value of the node in the map
        //! to know that the node is marked to be deleted
        void markToDelete(Node* node);

        //! This function must be called in the destructor of the Node class
        void unregisterAliveNode(Node* node);

    private:
        friend class Node;
        static Application* m_instance;

        std::vector<Node*> m_rootNodes;
        std::thread::id m_threadId;

        std::vector<Node*> m_rootNodesToDelete;

        //! This map is used to keep the nodes alive. The boolean value is used to know if
        //! the node is marked to be deleted.
        std::unordered_map<Node*, bool> m_aliveNodes;

        bool m_quit = false;
    };
} // namespace IRC

#endif // APPLICATION_H
