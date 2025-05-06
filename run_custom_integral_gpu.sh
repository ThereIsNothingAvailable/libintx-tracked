#!/bin/bash
#SBATCH --job-name=libintx_test
#SBATCH --partition=ice-gpu
#SBATCH --gres=gpu:1
#SBATCH --cpus-per-task=4
#SBATCH --mem=8G
#SBATCH --time=00:30:00
#SBATCH --output=custom_integral.out
#SBATCH --error=custom_integral.err

# Load CUDA
module purge
module load cuda/12.6.1

# Navigate to your build directory
cd ~/libintx/build

# Run the test
ctest -R custom_integral --output-on-failure
