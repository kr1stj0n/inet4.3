//
// author: Kr1stj0n C1k0
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
                datarate = 100Mbps;
                delay = 3ms;
                per = 0;
                ber = 0;
                thruputDisplayFormat = default("b B U");
        }
        channel lan extends ThruputMeteringChannel
        {
            parameters:
                datarate = 100Mbps;
                delay = 3ms;
                per = 0;
                ber = 0;
                thruputDisplayFormat = default("b B U");
        }

    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=523.8462,35.384617");
        }
        client11: StandardHost {
            @display("p=60.0,99.28");
        }
        client12: StandardHost {
            @display("p=60.0,190.384");
        }
        client13: StandardHost {
            @display("p=60.0,266.304");
        }
        client14: StandardHost {
            @display("p=60.0,339.888");
        }
        client15: StandardHost {
            @display("p=60.0,422.81598");
        }
        client21: StandardHost {
            @display("p=600.0,114.464");
        }
        client22: StandardHost {
            @display("p=600.0,190.384");
        }
        client23: StandardHost {
            @display("p=600.0,265.136");
        }
        client24: StandardHost {
            @display("p=600.0,339.888");
        }
        client25: StandardHost {
            @display("p=600.0,422.81598");
        }
        routerCoreA: Router {
            parameters:
                @display("p=241.776,263.968;i=abstract/router");
        }
        routerCoreB: Router {
            parameters:
                @display("p=430.992,263.968;i=abstract/router");
        }
    connections:
        client11.ethg++ <--> lan <--> routerCoreA.ethg++;
        client12.ethg++ <--> lan <--> routerCoreA.ethg++;
        client13.ethg++ <--> lan <--> routerCoreA.ethg++;
        client14.ethg++ <--> lan <--> routerCoreA.ethg++;
        client15.ethg++ <--> lan <--> routerCoreA.ethg++;
        routerCoreA.pppg++ <--> wan <--> routerCoreB.pppg++;
        routerCoreB.ethg++ <--> lan <--> client21.ethg++;
        routerCoreB.ethg++ <--> lan <--> client22.ethg++;
        routerCoreB.ethg++ <--> lan <--> client23.ethg++;
        routerCoreB.ethg++ <--> lan <--> client24.ethg++;
        routerCoreB.ethg++ <--> lan <--> client25.ethg++;
}
