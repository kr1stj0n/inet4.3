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
        channel NormalP extends DatarateChannel
        {
            parameters:
                datarate = 100Mbps;
                delay = 3ms;
                per = 0;
                ber = 0;
        }
        channel H2SW extends ThruputMeteringChannel
        {
            parameters:
                datarate = 1000Mbps;
                delay = 1ms;
                per = 0;
                ber = 0;
                thruputDisplayFormat = default("B");
        }

    submodules:
        routerCore: Router {
            parameters:
                @display("p=434.496,207.90399;i=abstract/router");
        }
        client11: StandardHost {
            @display("p=73.584,154.176");
        }
        client12: StandardHost {
            @display("p=73.584,254.624");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=523.8462,35.384617");
        }
        client21: StandardHost {
            @display("p=580.496,207.90399");
        }
        ethernetSwitch: EthernetSwitch {
            @display("p=287.328,206.736");
        }
    connections:
        client11.ethg++ <--> H2SW <--> ethernetSwitch.ethg++;
        client12.ethg++ <--> H2SW <--> ethernetSwitch.ethg++;
        ethernetSwitch.ethg++ <--> NormalP <--> routerCore.ethg++;
        routerCore.ethg++ <--> NormalP <--> client21.ethg++;
}

