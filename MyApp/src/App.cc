#include "App.hpp"
#include <runos/core/logging.hpp>

namespace runos {

    REGISTER_APPLICATION(App, {"topology", ""})

    void App::init(Loader *, const Config& ) {
        LOG(INFO) << "::: App initialization :::";
        connect(topo, &Topology::ready, this, &App::handle_signal);
    }
    
    void App::handle_signal()  {
        LOG(INFO) << "::: ComputePath by App :::";
        auto route_id = topo->newRoute(0, 2);
    }

} // namespace runos