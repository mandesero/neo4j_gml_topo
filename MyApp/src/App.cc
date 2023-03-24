#include "App.hpp"


namespace runos {

    void YourApp::init(Loader *, const Config& )
    {
        LOG(INFO) << "Init App";
        connect(topo, &Topology::ready, this, &MyApp::handle_signal);
    }

} // namespace runos