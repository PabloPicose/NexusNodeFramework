#pragma once

#include <Core/Node.h>
namespace IRC {
    class GUIWindow : public Node {
        public:
            explicit GUIWindow(Node* parent = nullptr);
            ~GUIWindow() override;

            void update() override;
        protected:
        private:
    };
}