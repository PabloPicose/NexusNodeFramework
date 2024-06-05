//
// Created by pablo on 5/28/24.
//

#ifndef NODE_H
#define NODE_H

namespace IRC {
    class Node {
    public:
        virtual ~Node();

        void setName(const std::string& name);

        std::string getName() const;

        /**
         * @brief Adds a child to the node and will take ownership of the child.
         * @details If the child is a root node it will be removed from the root nodes list.
         * @note If the child had been previously created as an object the use of deleteLater in
         * the child is not allowed and the program will crash.
         * For example, creating a Node myChild; and then add this child to another node will crash
         * in the moment of deleteLater moment.
         * @param child A valid pointer. The child must not be nullptr.
         */
        void addChild(Node* child);

        Node* getChild(std::size_t index) const;

        bool isChild(Node* child) const;

        void deleteLater();

        /**
         * This function is called from the Application. Can be called to instantly
         * update the node. There is not a loop inside.
         */
        void run();

        void setParent(Node* parent);

        Node* parent() const;

        std::size_t childrenCount() const;

        bool isRoot() const;

        size_t childrenToDeleteCount() const;

    protected:
        explicit Node(Node* parent = nullptr);

        virtual void update() = 0;

    private:
        void pushDeleteChild(Node* child);

    private:
        Node* m_parent = nullptr;
        std::vector<Node*> m_children;
        std::vector<Node*> m_childrenToDelete;
        bool m_isRoot = false;
        std::string m_name;
    };
} // namespace IRC

#endif // NODE_H
