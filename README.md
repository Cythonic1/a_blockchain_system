<h1> simple blockchain network</h1>
<h3>a blockchain network where you can generate blocks connect multiple devices in decentralized manner</h3>
<hr>
<h3>TODOs</h3>
<ul>
  <li>adding serialization and deserialization for blocks and network Packets (done)</li>
  <li>adding encryption and digital signature </li>
  <li>adding proof of work or (PoW)</li>
  <li>manage the memory more efficiently and avoid memory leaks</li>
  <li>implement the network protocol</li>
  <li>implement multi threading for connectors and listeners/li>
</ul>
<hr>
<h3>How to run</h3>

```bash
git clone https://github.com/Cythonic1/blockchain-block-creator.git
cd blockchain-block-creator/src
make && ./blockchain

```
<hr>
<h3>Updates</h3>
<ul>
  <li>Implement some important functions regarding the network communication</li>
  <li>Implement structures to manage the sockets also a strctures to manage the connected nodes</li>
  <li>Implement broadcast function to broadcast a network serialized block</li>
  <li> Implement UDP host discovery </li>
</ul>

