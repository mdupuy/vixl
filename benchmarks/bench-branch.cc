// Copyright 2013, ARM Limited
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//   * Neither the name of ARM Limited nor the names of its contributors may be
//     used to endorse or promote products derived from this software without
//     specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "a64/macro-assembler-a64.h"
#include "a64/instructions-a64.h"
#include "globals.h"

using namespace vixl;

static const unsigned kDefaultInstructionCount = 100000;

// This program focuses on emitting branch instructions.
//
// This code will emit a given number of branch immediate to the next
// instructions in a fixed size buffer, looping over the buffer if necessary.
// This code therefore focuses on Emit and label binding/patching.
int main(int argc, char* argv[]) {
  unsigned instructions = 0;

  switch (argc) {
    case 1: instructions = kDefaultInstructionCount; break;
    case 2: instructions = atoi(argv[1]); break;
    default:
      printf("Usage: %s [#instructions]\n", argv[0]);
      exit(1);
  }

  const unsigned buffer_size = 256 * KBytes;
  // Emitting on the last word of the buffer will trigger an assert.
  const unsigned buffer_instruction_count = buffer_size / kInstructionSize - 1;

  byte* assm_buffer = new byte[buffer_size];
  MacroAssembler* masm = new MacroAssembler(assm_buffer, buffer_size);

  #define __ masm->
  // We emit a branch to the next instruction.

  unsigned rounds = instructions / buffer_instruction_count;
  for (unsigned i = 0; i < rounds; ++i) {
    for (unsigned j = 0; j < buffer_instruction_count; ++j) {
      Label target;
      __ b(&target);
      __ bind(&target);
    }
    masm->Reset();
  }

  unsigned remaining = instructions % buffer_instruction_count;
  for (unsigned i = 0; i < remaining; ++i) {
    Label target;
    __ b(&target);
    __ bind(&target);
  }

  delete masm;
  delete assm_buffer;

  return 0;
}
