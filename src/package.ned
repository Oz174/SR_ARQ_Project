package sr_arq_project;

@license(LGPL);
//
// TODO documentation
//
network Network
{
    @display("bgb=627,303");
    submodules:
        coordinator: Coordinator {
            @display("p=277,194;i=block/cogwheel");
        }
        Node0: Node {
            @display("p=122,65;i=device/switch");
        }
        Node1: Node {
            @display("p=444,65;i=device/switch");
        }
    connections:
        coordinator.gate0 --> Node0.coordinator_gate;
        coordinator.gate1 --> Node1.coordinator_gate;
        Node0.out --> Node1.in;
        Node1.out --> Node0.in;
}

