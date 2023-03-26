#include "App.hpp"


namespace runos {

    void App::init(Loader *, const Config& ) {
        connect(topo, &Topology::ready, this, &MyApp::handle_signal);
    }
    
    void App::handle_signal()  {
        auto route_id = topo->newRoute(0, 2);
    }

} // namespace runos