#pragma once

namespace psm
{
    class Engine
    {
        //singleton realization
    public:
        Engine(Engine&) = delete;
        void operator=(const Engine&) = delete;
        static Engine* Instance();
    private:
        Engine();
        static Engine* s_Instance;
    //class specific
    public:
        void Init(int width, int height);
        void Dinit();
    };
}