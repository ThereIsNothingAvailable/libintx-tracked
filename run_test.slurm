#!/bin/bash
#SBATCH --job-name=libintx_md4_test
#SBATCH --partition=ice-gpu
#SBATCH --nodelist=atl1-1-02-010-34-0
#SBATCH --gres=gpu:1
#SBATCH --cpus-per-task=4
#SBATCH --mem=8G
#SBATCH --time=00:30:00
#SBATCH --output=libintx_md4.out
#SBATCH --error=libintx_md4.err

# Load CUDA
module purge
module load cuda/12.6.1

# Navigate to your build directory
cd ~/libintx/build

# Run the libintx.gpu.md4 test
ctest -R libintx.gpu.md4 --output-on-failure
