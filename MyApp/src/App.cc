#include "App.hpp"
#include <runos/core/logging.hpp>
#include "SwitchManager.hpp"

#include <chrono>


namespace runos {

    REGISTER_APPLICATION(App, {"topology", ""})

    void App::init(Loader *loader, const Config& ) {
        LOG(INFO) << "::: App initialization :::";
        topo = Topology::get(loader);
        connect(topo, &Topology::ready, this, &App::handle_signal);

        auto sw_mgr = SwitchManager::get(loader);
        // connect(sw_mgr, &SwitchManager::switchUp,
        //         this, &App::handle_signal);
        // connect(sw_mgr, &SwitchManager::switchDown,
        //         this, &App::handle_signal);
        // connect(sw_mgr, &SwitchManager::linkUp,
        //         this, &App::handle_signal);
        connect(sw_mgr, &SwitchManager::linkDown,
                this, &App::handle_signal);
    }
    
    void App::startUp(Loader* loader) {
        LOG(INFO) << "::: App startUp :::";
    }

    void App::handle_signal()  {
        LOG(INFO) << "::: ComputePath by App :::";
        
        std::chrono::high_resolution_clock::time_point start_time, end_time;
        start_time = std::chrono::high_resolution_clock::now();

        auto route_id = topo->newRoute(1, 2);

        end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        LOG(INFO) << "Find shortestPath time: " << duration.count() << " nanoseconds";
    }

} // namespace runos