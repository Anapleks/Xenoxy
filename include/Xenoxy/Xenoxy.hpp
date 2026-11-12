#ifndef Xenoxy_hpp
#define Xenoxy_hpp
#include <string>
#include "Xenoxy/impl/Types.hpp"

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
        ObjectInst Run(std::string function="main");
    };
}

#endif