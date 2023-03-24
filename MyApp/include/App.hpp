#include "Application.hpp"
#include "Loader.hpp"

namespace runos {

    class App : public Application
    {
        Q_OBJECT
        SIMPLE_APPLICATION(App, "your-app")
        
    public:
        void init(Loader* loader, const Config& config) override;

    public slots:
        void handle_signal() {
            auto route_id = topo->newRoute(0, 2)
        }
    };

} // namespace runos