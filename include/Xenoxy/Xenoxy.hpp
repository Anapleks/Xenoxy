#ifndef Xenoxy_hpp
#define Xenoxy_hpp
#include <string>

namespace Xenoxy {
    class Runtime {
        private:
        class RuntimeImpl;
        RuntimeImpl* pImpl;

        public:
        Runtime();
        ~Runtime();
        void Load(std::string code);
        void LoadFile(std::string path);
        void Run(std::string function="main");
    };
}

#endif