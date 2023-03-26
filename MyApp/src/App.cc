#include "App.hpp"
#include <runos/core/logging.hpp>

namespace runos {

    REGISTER_APPLICATION(App, {"topology", ""})

    void App::init(Loader *loader, const Config& ) {
        LOG(INFO) << "::: App initialization :::";
        topo = Topology::get(loader);
        connect(topo, &Topology::ready, this, &App::handle_signal);
    }
    
    void App::startUp(Loader* loader) {
        LOG(INFO) << "::: App startUp :::";
    }

    void App::handle_signal()  {
        LOG(INFO) << "::: ComputePath by App :::";
        auto route_id = topo->newRoute(0, 2);
    }

} // namespace runos