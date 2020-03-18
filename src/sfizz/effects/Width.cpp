// SPDX-License-Identifier: BSD-2-Clause

// This code is part of the sfizz library and is licensed under a BSD 2-clause
// license. You should have receive a LICENSE.md file along with the code.
// If not, contact the sfizz maintainers at https://github.com/sfztools/sfizz


/**
   Note(jpc): this effect is book-only, mentioned but not documented

   Note(jpc): implementation status

- [x] width
- [ ] width_oncc
 */

#include "Width.h"
#include "Opcode.h"
#include "SIMDHelpers.h"
#include "absl/memory/memory.h"

namespace sfz {
namespace fx {

    void Width::setSampleRate(double sampleRate)
    {
        (void)sampleRate;
    }

    void Width::setSamplesPerBlock(int samplesPerBlock)
    {
        _tempBuffer.resize(samplesPerBlock);
    }

    void Width::clear()
    {
    }

    void Width::process(const float* const inputs[], float* const outputs[], unsigned nframes)
    {
        const float baseWidth = _width;

        absl::Span<float> widths = _tempBuffer.getSpan(0);
        std::fill(widths.begin(), widths.end(), baseWidth);

        absl::Span<const float> input1 { inputs[0], nframes };
        absl::Span<const float> input2 { inputs[1], nframes };
        absl::Span<float> output1 { outputs[0], nframes };
        absl::Span<float> output2 { outputs[1], nframes };

        for (unsigned i = 0; i < nframes; ++i) {
            float l = input1[i];
            float r = input2[i];

            float w = clamp((widths[i] + 100.0f) * 0.005f, 0.0f, 1.0f);
            float coeff1 = _internals::panLookup(w);
            float coeff2 = _internals::panLookup(1.0f - w);

            output1[i] = l * coeff2 + r * coeff1;
            output2[i] = l * coeff1 + r * coeff2;
        }
    }

    std::unique_ptr<Effect> Width::makeInstance(absl::Span<const Opcode> members)
    {
        auto fx = absl::make_unique<Width>();

        for (const Opcode& opc : members) {
            switch (opc.lettersOnlyHash) {
            case hash("width"):
                setValueFromOpcode(opc, fx->_width, {-100.0f, 100.0f});
                break;
            }
        }

        return CXX11_MOVE(fx);
    }

} // namespace fx
} // namespace sfz
