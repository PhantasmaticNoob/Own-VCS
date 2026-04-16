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
 
Getting Started
Prerequisites

sudo apt update && sudo apt install -y gcc build-essential libssl-dev

Using This Repository

This is a template repository. Do not fork it.

    Click "Use this template" → "Create a new repository" on GitHub
    Name your repository (e.g., SRN-pes-vcs) and set it to public. Replace SRN with your actual SRN, e.g., PESXUG24CSYYY-pes-vcs
    Clone this repository to your local machine and do all your lab work inside this directory.
    Important: Remember to commit frequently as you progress. You are required to have a minimum of 5 detailed commits per phase. Refer to Submission Requirements for more details.
    Clone your new repository and start working

The repository contains skeleton source files with // TODO markers where you need to write code. Functions marked // PROVIDED are complete — do not modify them.
Building

make          # Build the pes binary
make all      # Build pes + test binaries
make clean    # Remove all build artifacts

Author Configuration

PES-VCS reads the author name from the PES_AUTHOR environment variable:

export PES_AUTHOR="Your Name <PESXUG24CS042>"

If unset, it defaults to "PES User <pes@localhost>".
File Inventory
File 	Role 	Your Task
pes.h 	Core data structures and constants 	Do not modify
object.c 	Content-addressable object store 	Implement object_write, object_read
tree.h 	Tree object interface 	Do not modify
tree.c 	Tree serialization and construction 	Implement tree_from_index
index.h 	Staging area interface 	Do not modify
index.c 	Staging area (text-based index file) 	Implement index_load, index_save, index_add
commit.h 	Commit object interface 	Do not modify
commit.c 	Commit creation and history 	Implement commit_create
pes.c 	CLI entry point and command dispatch 	Do not modify
test_objects.c 	Phase 1 test program 	Do not modify
test_tree.c 	Phase 2 test program 	Do not modify
test_sequence.sh 	End-to-end integration test 	Do not modify
Makefile 	Build system 	Do not modify
Understanding Git: What You're Building

Before writing code, understand how Git works under the hood. Git is a content-addressable filesystem with a few clever data structures on top. Everything in this lab is based on Git's real design.
The Big Picture

When you run git commit, Git doesn't store "changes" or "diffs." It stores complete snapshots of your entire project. Git uses two tricks to make this efficient:

    Content-addressable storage: Every file is stored by the SHA hash of its contents. Same content = same hash = stored only once.
    Tree structures: Directories are stored as "tree" objects that point to file contents, so unchanged files are just pointers to existing data.

Your project at commit A:          Your project at commit B:
                                   (only README changed)

    root/                              root/
    ├── README.md  ─────┐              ├── README.md  ─────┐
    ├── src/            │              ├── src/            │
    │   └── main.c ─────┼─┐            │   └── main.c ─────┼─┐
    └── Makefile ───────┼─┼─┐          └── Makefile ───────┼─┼─┐
                        │ │ │                              │ │ │
                        ▼ ▼ ▼                              ▼ ▼ ▼
    Object Store:       ┌─────────────────────────────────────────────┐
                        │  a1b2c3 (README v1)    ← only this is new   │
                        │  d4e5f6 (README v2)                         │
                        │  789abc (main.c)       ← shared by both!    │
                        │  fedcba (Makefile)     ← shared by both!    │
                        └─────────────────────────────────────────────┘

The Three Object Types
1. Blob (Binary Large Object)

A blob is just file contents. No filename, no permissions — just the raw bytes.

blob 16\0Hello, World!\n
     ↑    ↑
     │    └── The actual file content
     └─────── Size in bytes

The blob is stored at a path determined by its SHA-256 hash. If two files have identical contents, they share one blob.
2. Tree

A tree represents a directory. It's a list of entries, each pointing to a blob (file) or another tree (subdirectory).

100644 blob a1b2c3d4... README.md
100755 blob e5f6a7b8... build.sh        ← executable file
040000 tree 9c0d1e2f... src             ← subdirectory
       ↑    ↑           ↑
       │    │           └── name
       │    └── hash of the object
       └─────── mode (permissions + type)

Mode values:

    100644 — regular file, not executable
    100755 — regular file, executable
    040000 — directory (tree)

3. Commit

A commit ties everything together. It points to a tree (the project snapshot) and contains metadata.

tree 9c0d1e2f3a4b5c6d7e8f9a0b1c2d3e4f5a6b7c8d
parent a1b2c3d4e5f6a7b8c9d0e1f2a3b4c5d6e7f8a9b0
author Alice <alice@example.com> 1699900000
committer Alice <alice@example.com> 1699900000

Add new feature

The parent pointer creates a linked list of history:

    C3 ──────► C2 ──────► C1 ──────► (no parent)
    │          │          │
    ▼          ▼          ▼
  Tree3      Tree2      Tree1

How Objects Connect

                    ┌─────────────────────────────────┐
                    │           COMMIT                │
                    │  tree: 7a3f...                  │
                    │  parent: 4b2e...                │
                    │  author: Alice                  │
                    │  message: "Add feature"         │
                    └─────────────┬───────────────────┘
                                  │
                                  ▼
                    ┌─────────────────────────────────┐
                    │         TREE (root)             │
                    │  100644 blob f1a2... README.md  │
                    │  040000 tree 8b3c... src        │
                    │  100644 blob 9d4e... Makefile   │
                    └──────┬──────────┬───────────────┘
                           │          │
              ┌────────────┘          └────────────┐
              ▼                                    ▼
┌─────────────────────────┐          ┌─────────────────────────┐
│      TREE (src)         │          │     BLOB (README.md)    │
│ 100644 blob a5f6 main.c │          │  # My Project           │
└───────────┬─────────────┘          └─────────────────────────┘
            ▼
       ┌────────┐
       │ BLOB   │
       │main.c  │
       └────────┘

References and HEAD

References are files that map human-readable names to commit hashes:

.pes/
├── HEAD                    # "ref: refs/heads/main"
└── refs/
    └── heads/
        └── main            # Contains: a1b2c3d4e5f6...

HEAD points to a branch name. The branch file contains the latest commit hash. When you commit:

    Git creates the new commit object (pointing to parent)
    Updates the branch file to contain the new commit's hash
    HEAD still points to the branch, so it "follows" automatically

Before commit:                    After commit:

HEAD ─► main ─► C2 ─► C1         HEAD ─► main ─► C3 ─► C2 ─► C1

The Index (Staging Area)

The index is the "preparation area" for the next commit. It tracks which files are staged.

Working Directory          Index               Repository (HEAD)
─────────────────         ─────────           ─────────────────
README.md (modified) ──── pes add ──► README.md (staged)
src/main.c                            src/main.c          ──► Last commit's
Makefile                               Makefile                snapshot

The workflow:

    pes add file.txt → computes blob hash, stores blob, updates index
    pes commit -m "msg" → builds tree from index, creates commit, updates branch ref

Content-Addressable Storage

Objects are named by their content's hash:

# Pseudocode
def store_object(content):
    hash = sha256(content)
    path = f".pes/objects/{hash[0:2]}/{hash[2:]}"
    write_file(path, content)
    return hash

This gives us:

    Deduplication: Identical files stored once
    Integrity: Hash verifies data isn't corrupted
    Immutability: Changing content = different hash = different object

Objects are sharded by the first two hex characters to avoid huge directories:

.pes/objects/
├── 2f/
│   └── 8a3b5c7d9e...
├── a1/
│   ├── 9c4e6f8a0b...
│   └── b2d4f6a8c0...
└── ff/
    └── 1234567890...

Exploring a Real Git Repository

You can inspect Git's internals yourself:

mkdir test-repo && cd test-repo && git init
echo "Hello" > hello.txt
git add hello.txt && git commit -m "First commit"

find .git/objects -type f          # See stored objects
git cat-file -t <hash>            # Show type: blob, tree, or commit
git cat-file -p <hash>            # Show contents
cat .git/HEAD                     # See what HEAD points to
cat .git/refs/heads/main          # See branch pointer

What You'll Build

PES-VCS implements five commands across four phases:

pes init              Create .pes/ repository structure
pes add <file>...     Stage files (hash + update index)
pes status            Show modified/staged/untracked files
pes commit -m <msg>   Create commit from staged files
pes log               Walk and display commit history

The .pes/ directory structure:

my_project/
├── .pes/
│   ├── objects/          # Content-addressable blob/tree/commit storage
│   │   ├── 2f/
│   │   │   └── 8a3b...   # Sharded by first 2 hex chars of hash
│   │   └── a1/
│   │       └── 9c4e...
│   ├── refs/
│   │   └── heads/
│   │       └── main      # Branch pointer (file containing commit hash)
│   ├── index             # Staging area (text file)
│   └── HEAD              # Current branch reference
└── (working directory files)

Architecture Overview

┌───────────────────────────────────────────────────────────────┐
│                      WORKING DIRECTORY                        │
│                  (actual files you edit)                       │
└───────────────────────────────────────────────────────────────┘
                              │
                        pes add <file>
                              ▼
┌───────────────────────────────────────────────────────────────┐
│                           INDEX                               │
│                (staged changes, ready to commit)              │
│                100644 a1b2c3... src/main.c                    │
└───────────────────────────────────────────────────────────────┘
                              │
                       pes commit -m "msg"
                              ▼
┌───────────────────────────────────────────────────────────────┐
│                       OBJECT STORE                            │
│  ┌───────┐    ┌───────┐    ┌────────┐                         │
│  │ BLOB  │◄───│ TREE  │◄───│ COMMIT │                         │
│  │(file) │    │(dir)  │    │(snap)  │                         │
│  └───────┘    └───────┘    └────────┘                         │
│  Stored at: .pes/objects/XX/YYY...                            │
└───────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌───────────────────────────────────────────────────────────────┐
│                           REFS                                │
│       .pes/refs/heads/main  →  commit hash                    │
│       .pes/HEAD             →  "ref: refs/heads/main"         │
└───────────────────────────────────────────────────────────────┘

