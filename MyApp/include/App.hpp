#include "Application.hpp"
#include "Loader.hpp"
#include "Topology.hpp"


namespace runos {

    class App : public Application
    {
        Q_OBJECT
        SIMPLE_APPLICATION(App, "App")
        
    public:
        Topology *topo;
        void init(Loader* loader, const Config& config) override;

    public slots:
        void handle_signal();
    };

} // namespace runos