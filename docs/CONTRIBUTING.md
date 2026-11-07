# Contributing

Thank you for your interest in contributing to ALSXT!

Please read this document in its entirety before performing a Pull Request.

### Requirements
1. You agree to the [Individual Contributor License Agreement (ICLA)](individual-contributor-license-agreement)
2. You agree to [the Code of Conduct](CODE_OF_CONDUCT.md)
3. You follow the [Pull Request Process](#pull-request-process) below

> [!NOTE]
> When contributing bug fixes to this repository, please first discuss the change you wish to make via issue, email, or any other method with the owners of this repository, as the issue may already be solved but not yet commited upstream.

### Pull Request Process

To maintain code quality and ensure a smooth review process for this UE5 C++ plugin, please follow these steps before submitting a Pull Request (PR):
#### 1. Sync Your Branch
Ensure your local branch is up to date with the latest changes from the main (or target) branch.
- Perform a git fetch and git rebase (or merge) from the upstream repository.
- Conflicts: Resolve any merge conflicts locally before pushing your PR.

#### 2. Pre-Submission Testing
All C++ changes must be verified to ensure they don't break the build or existing features:
-  Compilation: Verify the plugin compiles successfully for Development Editor and Shipping configurations.
- Manual Verification: Test the changes within the Unreal Editor. If your PR introduces a new feature, please include a brief video or screenshot in the PR description showing it in action.
- CI/CD: If our repository has automated GitHub Actions/CI enabled, ensure all checks pass (green tick) after you push your branch.

#### 3. Descriptive Commits & PRs
We require clear documentation of what changed and why.
- Commit Messages: Use the imperative mood (e.g., "Add safe pointer check to UMyObject"). Avoid vague messages like "fixed bug" or "updated code".
- PR Description: Use the provided template to explain:
   - The problem being solved.
   - The technical approach taken.
   - Any specific UE5 modules or dependencies added.

### 4. Sign the CLA
By submitting a Pull Request, you agree to our Individual Contributor License Agreement. If you haven't signed it yet, our CLA bot will prompt you to do so in the PR comments.

### How to Submit

- Fork the repository and create your feature branch.
- Commit your changes following the descriptive guidelines above.
- Push to your fork and Open a Pull Request against our main branch.
- Wait for a maintainer to review your code. We may request changes before merging.


### Individual Contributor License Agreement

**Individual Contributor License Agreement ("Agreement")**

Thank you for your interest in ALSXT (the "Project"). 

This Agreement outlines the intellectual property rights for contributions, ensuring the Maintainer can utilize submissions in both open-source and commercial contexts. 

#### 1. Definitions
- "You": The copyright owner or entity making the submission.
- "Contribution": Any work intentionally submitted for inclusion in the Project.

#### 2. Copyright & Patent License
You grant a perpetual, worldwide, non-exclusive, no-charge, royalty-free, irrevocable license to use, reproduce, modify, display, sublicense, and sell your Contributions in the Project, including in commercial or proprietary versions.

#### 3. Commercial Use
The Maintainer may include your contribution in commercial, proprietary, or "Marketplace" versions of the project and sublicense it under different terms.

#### 4. Representations & Liability
You represent that you are legally entitled to grant these licenses and that your contributions are provided "AS IS," without warranties, with limitations of liability for both parties. 
