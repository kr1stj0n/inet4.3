//
// author: Marcel Marek
//

package inet.examples.inet.lgc;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import ned.DatarateChannel;


network LgcIncast
{
    parameters:
        @display("bgb=740,580");

    types:
        channel NormalP extends DatarateChannel
        {
            parameters:
                datarate = 100Mbps;
                delay    = 3ms;
                per      = 0;
                ber      = 0;
        }

    submodules:
        routerCore: Router {
            parameters:
                @display("p=434.496,140.16;i=abstract/router");
        }
        client12: StandardHost {
            @display("p=50,176");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=523.8462,35.384617");
        }
        client21: StandardHost {
            @display("p=543.12,135.48799");
        }
    connections:
        client12.pppg++ <--> NormalP <--> routerCore.pppg++;
        routerCore.pppg++ <--> NormalP <--> client21.pppg++;
}
