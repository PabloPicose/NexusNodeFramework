#pragma once 

#include "Core/Node.h"

namespace IRC {
    class MainWindow;

    class GUIBase : public Node {
    public:
        ~GUIBase() override;

    protected:
        GUIBase(MainWindow* parent);

        void update() override final;
    private:
        void startFrame();

        void renderFrame();

        void renderDrawData();
    };

}