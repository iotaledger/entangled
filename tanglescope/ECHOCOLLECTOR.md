tanglescope/EchoCatcher
================================

Below is an outdated description of what EchoCatcher is supposed to do.
Instead of creating an IXI out of it (which might very well happen in the future), we'll stick to having a standalone service for now.

```
*Echo Catcher IXI*
It's an IXI module whose purpose is to measure quality of the network by issuing transactions and tracking their propagation through other nodes with installed Echo Catcher. Measurement data can be used for network topology improvement by proposing the owner of the "farthest" node to become the neighbors and warning the owner of the "closest" node that his node will be removed from the neighbors list. Note, that every measurement cycle shouldn't lead to addition/removal of more than a single node.
Brief description of the logic:
- Alice generates a special transaction containing `udp://IP:portUsedSolelyForEchoCatcher` and tag "ECHOCATCHER" and attaches it to the tangle with a pretty high minWeightMagnitude
- the others send a special UDP packet to `udp://IP:portUsedSolelyForEchoCatcher` right after they receive Alice's transaction, the packet must contain the contact info of the node operator
- Alice measures interval between broadcasting a transaction and receiving its "echo"
- the measurement is repeated several times with other transactions to collect average echo intervals
```
