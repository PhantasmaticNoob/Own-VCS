# PES VCS Lab Submission

## Student Details
Name: Vallabha Anand Bhilare  
SRN: PES1UG24AM414

---
## ScreenShots required
1A 
<img width="926" height="387" alt="1A" src="https://github.com/user-attachments/assets/c958aad5-63fa-4b47-ade4-5841a7be7690" />

1B
<img width="937" height="90" alt="1B" src="https://github.com/user-attachments/assets/99222cf8-e3bb-4d35-b0ba-7da41fc5c2b7" />

2A
<img width="937" height="301" alt="2A" src="https://github.com/user-attachments/assets/72690696-ad1a-4c6f-a91c-ad707abd381b" />

2B
<img width="931" height="431" alt="2B" src="https://github.com/user-attachments/assets/a0efc47b-5ca4-41f8-a289-74cea02a8359" />

3A
<img width="929" height="847" alt="3A" src="https://github.com/user-attachments/assets/732bf5a7-93d9-4831-b5ef-2e96e2e51826" />

3B
<img width="928" height="328" alt="3B" src="https://github.com/user-attachments/assets/0d06f9e1-6d16-4f55-a055-99330fb33c89" />

4A
<img width="951" height="1001" alt="4A" src="https://github.com/user-attachments/assets/e55ccda3-2d55-4c32-8deb-a7c3471ed36e" />

4B
<img width="946" height="791" alt="4B" src="https://github.com/user-attachments/assets/cae15760-b490-4ac9-bc58-4866c4e49faf" />

4C
<img width="954" height="361" alt="4C" src="https://github.com/user-attachments/assets/79f4901d-5678-410e-b16e-859e6453fb76" />

INTEGRATION TESTS
<img width="954" height="957" alt="IntegrationTest1" src="https://github.com/user-attachments/assets/01f49416-e95c-4d71-8679-ae4a295009c1" />
<img width="945" height="1000" alt="IntegrationTest2" src="https://github.com/user-attachments/assets/bf516286-4e08-4461-91c5-e667022686c8" />

---
## Implementation

This project implements a simplified version control system with:

- Content-addressable object store (object.c)
- Tree construction and serialization (tree.c)
- Index/staging area (index.c)
- Commit creation and history traversal (commit.c)

---

## Features Implemented

- Blob storage using SHA-256
- Tree objects for directory structure
- Index for staging files
- Commit objects with parent tracking
- Log traversal

---

## Analysis Questions

# Phase 5: Branching and Checkout

## Q5.1 — Implementing `pes checkout <branch>`

A branch is stored as a file inside `.pes/refs/heads/<branch>` containing a commit hash. Implementing checkout requires updating repository metadata and reconstructing the working directory.

### Files to update in `.pes/`
- `.pes/HEAD`  
  Update it to: ref: refs/heads/<branch>
- The branch file itself does not change.

### Working directory updates
1. Read the commit hash from `.pes/refs/heads/<branch>`.
2. Load the corresponding commit object.
3. Extract its root tree.
4. Recursively reconstruct the directory:
 - Create directories
 - Restore files from blob objects
5. Remove files not present in the target tree.

### Why this is complex
- Requires rebuilding the full filesystem state.
- Must handle file additions, deletions, and modifications.
- Must avoid overwriting uncommitted changes.
- Requires recursive traversal of tree objects.
- Must keep working directory, index, and object store consistent.

---

## Q5.2 — Detecting Dirty Working Directory Conflicts

Checkout must fail if uncommitted changes would be overwritten.

### Detection method
For each file in the index:
1. Use `stat()` to compare:
 - modification time (`mtime`)
 - file size
2. If different:
 - Recompute the file’s hash
 - Compare with the stored index hash

If hashes differ → file is modified (dirty)

### Conflict condition
A conflict exists if:
- File is modified in working directory, AND
- The same file differs between current branch and target branch

### Action
- If conflict exists → abort checkout
- Otherwise → proceed safely

---

## Q5.3 — Detached HEAD

### Definition
A detached HEAD occurs when `.pes/HEAD` contains a commit hash instead of a branch reference.

### Behavior during commits
- New commits are created normally.
- No branch is updated.
- Commits become unreachable once HEAD moves away.

### Recovery methods
- Create a new branch: pes branch <new-branch>
- Or manually reference the commit hash: pes branch recovered <commit-hash>


---

# Phase 6: Garbage Collection and Space Reclamation

## Q6.1 — Finding and Deleting Unreachable Objects

### Goal
Remove objects not reachable from any branch.

### Algorithm
1. Initialize an empty set: reachable = {}
2. For each branch:
- Read commit hash
- Traverse recursively:
  - Mark commit
  - Mark tree
  - Mark blobs
  - Follow parent commits
3. After traversal:
- Scan `.pes/objects/`
- Delete objects not in `reachable`

### Data structure
- Use a **hash set** for O(1) lookup of object IDs.

### Estimated objects
For:
- 100,000 commits
- 50 branches

Approximate:
- 100,000 commits
- 100,000 trees
- 200,000–500,000 blobs

Total traversal:
**~300,000 to 700,000 objects**

---

## Q6.2 — Dangers of Concurrent Garbage Collection

### Race condition
1. Commit operation writes new objects.
2. GC runs simultaneously.
3. New objects are not yet referenced.
4. GC deletes them.
5. Commit then references deleted objects.

Result: repository corruption.

### How Git avoids this
- Uses reference-based reachability checks.
- Maintains reflogs to keep recent commits alive.
- Applies delayed deletion (grace period).
- Uses file locking during GC.
- Ensures atomic updates of references.

### Key idea
Objects are never deleted if they might still become reachable.
 

