# RDAI Usage Instructions
[What is RDAI?](README.md)

## Installation
Installation instructions can be found [here](INSTALL.md).

## Applications
Applications are split into two folders in `Halide-to-Hardware/apps/hardware_benchmarks`:
- `tests` features simpler applications like conv_3_3.
- `apps` features more complex applications like resnet.

## Running the Pipeline
In order to run an application through the pipeline on a hardware platform, follow these steps:
1. Change into the application directory: `cd apps/hardware_benchmarks/path/to/app/`
2. In the Makefile, change `RDAI_PLATFORM_RUNTIME` to the desired platform runtime (e.g. `clockwork_sim`). Available platform runtimes are located in `rdai/platform_runtimes`.
3. Generate target design: `make compiler && make <design>`
4. Run input through target design: `make run-<design>`
5. Compare output to cpu output: `make compare-<design>`

Here is a list of the different make targets:
<pre><code>make clean               # remove generated files (bin directory)
     compiler            # compile updates to Halide compiler
     generator           # create Halide generator
     cpu                 # create CPU design
     clockwork           # create clockwork design
     image               # create an image with random data
     run-cpu             # create output file using CPU implementation
     run-clockwork       # create output file using clockwork implementation
     compare-clockwork   # compare Clockwork output file to CPU output image
     eval                # evaluate runtime </code></pre>

The definition of all of these targets can be found in `apps/hardware_benchmarks/hw_support/hardware_targets.mk`.

## Halide-to-Hardware Repository Tree:
<pre><code>Halide-to-Hardware
└── apps
    └── hardware_benchmarks                    // contains simpler test cases
        ├── apps                               // contains all apps compiled to coreir
        └── tests                              // contains all simpler test cases
            └── conv_3_3                       // one of the apps: does 3x3 convolution
                ├── Makefile                   // specifies commands for 'make'
                ├── conv_3_3_generator.cpp     // contains Halide algorithm and schedule
                ├── input.png                  // input image for testing
                ├── process.cpp                // runs input image with design to create ouput image
                ├── golden                     // this holds all expected output files
                │   └── golden_output.png      // output image expected
                │
                └── bin                        //// Running 'make clockwork' generates in this folder:
                    ├── output_cpu.png         // output image created using CPU implementation
                    └── output_clockwork.png   // output image created during testing; should be equivalent to output_cpu.png</code></pre>
