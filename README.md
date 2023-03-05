# Hierarchical task network planner
## TO-DO
- [X] Basic building blocks
  - [X] Primitive Tasks
  - [X] Compound Tasks
- [X] Override Brain Component
  - [X] Handle world state
  - [X] Handle tasks
  - [X] Handle domain
  - [X] Handle messages
- [X] Planner
  - [X] Basic planning
  - [X] Restore points
- [X] Planner runner
  - [X] Multi thread or distributed in multiple frames?
  - [X] Plan queue
- [ ] Observe world state changes for re-plan
  - [X] Implement basic observers
  - [ ] Add filter to observe only relevant tasks

## Improvements
- [ ] Custom instancing of domain
  - [ ] Give each task a separate memory as behavior trees
- [ ] Partial planning
  - [ ] Tasks that let the planner pause
- [ ] Sub domains -> execute domains inside other domains
- [ ] Slots -> insert domains dynamically
- [ ] Support to run multiple HTNs in the same AI
- [ ] Custom editor for domain construction
- [ ] GOAP planning support
