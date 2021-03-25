//
// author: Marcel Marek
//

package inet.examples.inet.shq;

import inet.common.misc.ThruputMeteringChannel;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.ethernet.EthernetSwitch;
import inet.queueing.filter.ShQDropper;
import inet.queueing.queue.ShQDropperQueue;
import inet.queueing.filter.RedDropper;
import inet.queueing.queue.RedDropperQueue;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import ned.DatarateChannel;


network ShQ
{
    parameters:
        @display("bgb=740,580");

    types:
        channel wan extends ThruputMeteringChannel
        {
            parameters:
                datarate = 1000Mbps;
                delay = 3ms;
                per = 0;
                ber = 0;
                thruputDisplayFormat = default("b B U");
        }
        channel lan extends ThruputMeteringChannel
        {
            parameters:
                datarate = 100Mbps;
                delay = 1ms;
                per = 0;
                ber = 0;
                thruputDisplayFormat = default("b B U");
        }

    submodules:
        routerCore: Router {
            parameters:
                @display("p=405.296,207.90399;i=abstract/router");
        }
        client11: StandardHost {
            @display("p=59.568,136.65599");
        }
        client12: StandardHost {
            @display("p=59.568,297.84");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=523.8462,35.384617");
        }
        client21: StandardHost {
            @display("p=685.61597,207.90399");
        }
        ethernetSwitch: EthernetSwitch {
            @display("p=184.54399,206.736");
        }
    connections:
        client11.ethg++ <--> lan <--> ethernetSwitch.ethg++;
        client12.ethg++ <--> lan <--> ethernetSwitch.ethg++;
        ethernetSwitch.ethg++ <--> lan <--> routerCore.ethg++;
        routerCore.ethg++ <--> wan <--> client21.ethg++;

}

