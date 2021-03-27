//
// author: Kr1stj0n C1k0
//

package inet.examples.inet.shq;

import inet.common.misc.ThruputMeteringChannel;
import ned.DatarateChannel;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.ethernet.EthernetSwitch;
import inet.queueing.filter.ShQDropper;
import inet.queueing.queue.ShQDropperQueue;
import inet.queueing.filter.RedDropper;
import inet.queueing.queue.RedDropperQueue;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;


network ShQ
{
    parameters:
        @display("bgb=740,580");
        int s1_numClients;
        int s2_numClients;
        int s3_numClients;
        int r1_numClients;
        int r2_numClients;

    types:
        channel wan extends ThruputMeteringChannel
        {
            parameters:
                datarate = 10000Mbps;
                delay = 0.01ms;
                per = 0;
                ber = 0;
                thruputDisplayFormat = default("b B U");
        }
        channel lan extends ThruputMeteringChannel
        {
            parameters:
                datarate = 1000Mbps;
                delay = 0.01ms;
                per = 0;
                ber = 0;
                thruputDisplayFormat = default("b B U");
        }

    submodules:
        configurator: Ipv4NetworkConfigurator;

        clients1[s1_numClients]: StandardHost;
        clients2[s2_numClients]: StandardHost;
        clients3[s3_numClients]: StandardHost;
        clientr1[r1_numClients]: StandardHost;
        clientr2[r2_numClients]: StandardHost;

        routerCoreA: Router;
        routerCoreB: Router;
    connections:
        for i=0..s1_numClients-1 {
            clients1[i].ethg++ <--> lan <--> routerCoreA.ethg++;
        }
        for i=0..s2_numClients-1 {
            clients2[i].ethg++ <--> lan <--> routerCoreA.ethg++;
        }
        for i=0..s3_numClients-1 {
            clients3[i].ethg++ <--> lan <--> routerCoreB.ethg++;
        }
        for i=0..r1_numClients-1 {
            clientr1[i].ethg++ <--> lan <--> routerCoreB.ethg++;
        }
        for i=0..r2_numClients-1 {
            clientr2[i].ethg++ <--> lan <--> routerCoreB.ethg++;
        }
        routerCoreA.pppg++ <--> wan <--> routerCoreB.pppg++;
}
