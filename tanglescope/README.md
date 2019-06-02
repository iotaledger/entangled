Tanglescope
===========

Tanglescope is a program composed of metric collectors,
Each collector is responsible for a different aspect of the Tangle behavior and collects different set of metrics
and exposes them to a configurable prometheus end point. (https://github.com/prometheus/prometheus) 

Configuration
==============

Each collector has its own section defined in a yaml file whose path is specified by `--ConfigurationPath` flag
(Default example yaml file in runner/configuration.yaml)

Collectors:
=================================

Stats collector
-------------------

**Purpose:**

Measuring transactions/bundles traffic data by listening to a list of zmq publisher (IRI nodes with `ZMQ_ENABLED = true`)
                 

**Configuration section example:**


```yaml
statscollector:

#IP / Port that the Prometheus Exposer binds to
  prometheus_exposer_uri: "0.0.0.0:8081"
#URL of ZMQ publisher to connect to
  publishers:
      - "tcp://zmq.testnet.iota.org:5556"

#histogram of bundle's confirmation duration granularity:
#(number of buckets = bundle_confirmation_histogram_range * 1000 / bundle_confirmation_bucket_size)

#histogram's range  [seconds]
  bundle_confirmation_histogram_range: 240
#bucket's size [seconds]
  bundle_confirmation_bucket_size: 2
```

**Metrics**:

- statscollector_bundles_confirmed (confirmed bundles count) [Counter]
- statscollector_value_transactions_new  (Value TXs count) [Counter]
- statscollector_bundles_new (new bundles count) [Counter]
- statscollector_transactions_confirmed (confirmed TXs count) [Counter]
- statscollector_value_transactions_confirmed  (Confirmed value TX's count) [Counter]
- statscollector_transactions_new (new TXs count) [Counter]
- statscollector_transactions_reattached (Reattached TXs count) [Counter]
- statscollector_value_confirmed (Confirmed tx's accumulated value) [Counter]
- statscollector_value_new (new tx's accumulated value) [Counter]
- statscollector_bundle_confirmation_duration (bundle's confirmation duration [seconds]) [Histogram]
- statscollector_to_broadcast (Number of transactions to broadcast to other nodes) [Gauge]
- statscollector_to_process (Number of transactions to process) [Gauge]
- statscollector_to_reply (Number of transactions to reply to nodes who requested) [Gauge]
- statscollector_to_request (Number of transactions to request from other nodes) [Gauge]
- statscollector_total_transactions (Number of transactions stored in node) [Counter]
- statscollector_milestones_count (Number of received milestones) [Counter]

Echo collector
---------------

**Purpose:**

Measuring the quality of the network by issuing transactions and tracking their propagation through other nodes
which has `ZMQ_ENABLED = true` (list of `publishers` in configuration section)

Measurement data can be used for network topology improvement by proposing the owner of the "farthest" node to become a
neighbor and warning the owner of the "closest" node that his node will be removed from the neighbors list.
Note,
Brief description of the logic:
- Alice generates a special transaction containing `udp://IP:portUsedSolelyForEchoCatcher` and tag "ECHOCATCHER" and attaches it to the tangle with a pretty high minWeightMagnitude
- the others send a special UDP packet to `udp://IP:portUsedSolelyForEchoCatcher` right after they receive Alice's transaction, the packet must contain the contact info of the node operator
- Alice measures interval between broadcasting a transaction and receiving its "echo"
- the measurement is repeated several times with other transactions to collect average echo intervals

**Configuration section example:**


```yaml
  echocollector:
#list of publishers to listen to.(they should return the echo)
  publishers:
      - "tcp://zmq.testnet.iota.org:5556"

#url of iri node
  iri_host: "iri01.testnet.iota.cafe"
  iri_port: 14265
#IP / Port that the Prometheus Exposer binds to
  prometheus_exposer_uri: "0.0.0.0:8080"
#MWM for transaction's POW
  mwm: 9
#in - between transactions interval[seconds]
  broadcast_interval: 20
#how often should echocatcher discover "unseen" transactions[seconds]
#shouldn't be too frequent since it can incur in multiple api
#calls..
  discovery_interval: 5  
```

Also requires:

```yaml
dbloader:

#URL of ZMQ publisher to connect to
  publisher: "tcp://zmq.testnet.iota.org:5556"
#TX's older than that age will be removed each cleanup_interval [seconds]
  oldest_tx_age: 1800
#in - between cleanup interval[seconds]
  cleanup_interval: 60
```

**Metrics**:

- echocollector_time_elapsed_arrived (Milliseconds it took for tx to arrive to destination) [Histogram]  
- echocollector_time_elapsed_received (Milliseconds it took for tx to travel back to transaction's original source) [Histogram]
- echocollector_time_elapsed_unseen_tx_published (Milliseconds it took for tx to be published since the moment client first learned about it) [Histogram]


Blowball collector
-------------------


**Purpose:**

Identifying blowballs which can indicate network problems/malice behavior.
By listening to a list of zmq publishers (IRI nodes with `ZMQ_ENABLED = true`)
and ref counting transaction's direct approvers

**Configuration section example:**
```yaml
blowballcollector:

#IP / Port that the Prometheus Exposer binds to
  prometheus_exposer_uri: "0.0.0.0:8082"
#URL of ZMQ publisher to connect to
  publisher: "tcp://zmq.testnet.iota.org:5556"
#interval between following snapshots(internal counters update)
  snapshot_interval: 60

#histogram granularity configuration
#histogram 's bucket' s size(refcount of tx approvers)
  bucket_size: 1
#histogram's range (maximum value of refcount to report)
  histogram_range: 40up_interval: 60
```

**Metrics**:

- blowball_tx_num_approvers (Number of transactions directly approving a single transaction) [Histogram]

Tip selection collector
------------------------

**Purpose:**

Monitoring tip selection's algorithm behavior.

**Configuration section example:**
```yaml
tipselectioncollector:

#IP / Port that the Prometheus Exposer binds to
  prometheus_exposer_uri: "0.0.0.0:8083"
#url of iri node
  iri_host : "iri01.testnet.iota.cafe"
  iri_port: 14265
#depth to give to "getTransactionsToApprove"
  depth: 10
#in - between samples interval[seconds]
  sample_interval: 20
#of calls to getTransactionsToApprove in each sample
  sample_size: 20
#histogram granularity configuration
#duration 's histogram (time it took ' getTransactionsToApprove ') bucket' s size[ms]
  duration_bucket_size: 20
#duration 's histogram range (maximum time it took ' getTransactionsToApprove')
  duration_histogram_range: 1000
```

**Metrics**:

- tipselection_num_trunk_eq_branch (# of times both selected tips were the same) [Histogram]
- tipselection_num_tx_was_not_a_tip (# of times selected tx was not a tip) [Histogram]
- tipselection_num_tx_was_selected (# of times tip/tx was selected) [Histogram]
- tipselection_tip_selection_duration (The duration_of_tip_selection) [Histogram]

Tangle's width collector
-------------------------


**Purpose:**

Measuring tangle width as defined below:
```
 Width definition (by Darcy Camargo):
 W(s,t) = |{(Vi,Vj) : Vi approves Vj, Vi > s, Vj < s}|
 't' is of course > 's' and the more t > s the more
 complete is the picture of the tangle in regards to
 the measure line s
 (basically, number of graph edges crossing a measure line/timepoint)
```


**Configuration section example:**


```yaml
tanglewidthcollector:

#IP / Port that the Prometheus Exposer binds to
  prometheus_exposer_uri: "0.0.0.0:8084"
#The measure line base from the above definition, [seconds]
  measure_line_base_age: 30
#The measure line max age[seconds]
  measure_line_max_age: 90
#Step from measure_line_base_age to measure_line_max_age, [seconds]
  measure_line_age_step: 10
#interval between following snapshots(internal counters update)
  snapshot_interval: 10
```

Also requires:

```yaml
dbloader:

#URL of ZMQ publisher to connect to
  publisher: "tcp://zmq.testnet.iota.org:5556"
#TX's older than that age will be removed each cleanup_interval [seconds]
  oldest_tx_age: 1800
#in - between cleanup interval[seconds]
  cleanup_interval: 60
```

**Metrics**:

- tanglewidthcollector_measure_line (Measure line's timestamp) [Gauge]  
- tanglewidthcollector_tangle_width (Width/Number of edges crossing a measure line) [Gauge]

Confirmation rate collector
---------------------------


**Purpose:**

Measuring transaction's confirmation rate


**Configuration section example:**


```yaml
confirmationratecollector:
#list of publisher to listen to.(they should return the echo)
  publishers:
      - "tcp://zmq.testnet.iota.org:5556"

#url of iri node
  iri_host: "iri01.testnet.iota.cafe"
  iri_port: 14265
#IP / Port that the Prometheus Exposer binds to
  prometheus_exposer_uri: "0.0.0.0:8085"
#MWM for transaction's POW
  mwm: 9
#in - between transactions interval[seconds]
  broadcast_interval: 3

#Time after which we should expect transaction to be approved
  measurement_upper_bound: 60
#Time after which if a transaction hasn't been approved, we give up
  measurement_lower_bound: 360
#should calculate confirmation rate using API calls
  enable_cr_from_api: true
#For checking the CR for various Latencies, define the latency in seconds,
#"step" means that for 10 seconds we will check latency of 10, 20, 30 seconds
  additional_latency_step_seconds: 10
#num different latencies to check
  additional_latency_num_steps: 3
```

**Metrics**:

- confirmationratecollector_confirmation_rate_api (Confirmation rate ratio [0,1] as it is perceived by making api calls to "getInclusionStates") [Gauge]  

- confirmationratecollector_confirmation_rate_zmq (Confirmation rate ratio [0,1] as it is perceived by inspecting the zmq stream) [Gauge]
