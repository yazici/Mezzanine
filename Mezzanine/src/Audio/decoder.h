// � Copyright 2010 - 2017 BlackTopp Studios Inc.
/* This file is part of The Mezzanine Engine.

    The Mezzanine Engine is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The Mezzanine Engine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with The Mezzanine Engine.  If not, see <http://www.gnu.org/licenses/>.
*/
/* The original authors have included a copy of the license specified above in the
   'Docs' folder. See 'gpl.txt'
*/
/* We welcome the use of the Mezzanine engine to anyone, including companies who wish to
   Build professional software and charge for their product.

   However there are some practical restrictions, so if your project involves
   any of the following you should contact us and we will try to work something
   out:
    - DRM or Copy Protection of any kind(except Copyrights)
    - Software Patents You Do Not Wish to Freely License
    - Any Kind of Linking to Non-GPL licensed Works
    - Are Currently In Violation of Another Copyright Holder's GPL License
    - If You want to change our code and not add a few hundred MB of stuff to
        your distribution

   These and other limitations could cause serious legal problems if you ignore
   them, so it is best to simply contact us or the Free Software Foundation, if
   you have any questions.

   Joseph Toppi - toppij@gmail.com
   John Blackwood - makoenergy02@gmail.com
*/
// Copyright (c) 2008-2010 Raynaldo (Wildicv) Rivera, Joshua (Dark_Kilauea) Jones
// This file is part of the "cAudio Engine"
// For conditions of distribution and use, see copyright notice in cAudio-ZLIBLicense.txt
#ifndef _audiodecoder_h
#define _audiodecoder_h

#include "datatypes.h"
#include "datastream.h"
#include "Audio/audioenumerations.h"

namespace Mezzanine
{
    namespace Audio
    {
        ///////////////////////////////////////////////////////////////////////////////
        /// @brief This is an interface class for the decoding of audio from a stream.
        ///////////////////////////////////////
        class iDecoder
        {
        protected:
            /// @brief Clears EoF and Fail bits from the stream if they are present.
            /// @remarks This should only check for the EoF bit, and if found clear EoF and Fail bits.  Both of these
            /// can be encountered when the stream reaches EoF and isn't an error condition when streaming audio.
            virtual void ClearStreamErrors() = 0;
        public:
            /// @brief Class constructor.
            iDecoder() {  }
            /// @brief Class destructor.
            virtual ~iDecoder() {  }

            ///////////////////////////////////////////////////////////////////////////////
            // Utility

            /// @brief Gets whether or not the decoder is ready to be used.
            /// @note On failure the issue likely lies with the stream being an improper format/encoding.
            /// @return Returns true if this decoder is ready for playback, false otherwise.
            virtual Boole IsValid() = 0;
            /// @brief Gets the encoding supported by this decoder.
            /// @return Returns an @ref Audio::Encoding value representing the encoding supported by this decoder.
            virtual Audio::Encoding GetEncoding() const = 0;
            /// @brief Gets whether or not seeking is supported.
            /// @return Returns true if you can skip to a specific point in the stream, false if you are stuck waiting.
            virtual Boole IsSeekingSupported() = 0;
            /// @brief Gets the Bit Configuration used to decode the audio stream.
            /// @return Returns the Bit Configuration currently being used to decode this stream.
            virtual Audio::BitConfig GetBitConfiguration() const = 0;
            /// @brief Gets the frequency used to decode the audio stream.
            /// @return Returns the frequency (or sample rate) currently being used to decode this stream.
            virtual UInt32 GetFrequency() const = 0;
            /// @brief Gets the stream being decoded.
            /// @return Returns a shared pointer to the DataStream being decoded.
            virtual DataStreamPtr GetStream() const = 0;
            /// @brief Checks to see if the decode has reached the end of the stream.
            /// @remarks Multiple decoders may use the same stream, and when this happens the actual underlying stream position
            /// may be altered multiple times to varying positions in the stream.  Because of this, checking the underlying
            /// stream directly may not give you an accurate idea of if this decoders point in the stream has reached it's end.
            /// That is what this function is meant to address.
            /// @return Returns true if all of the data in the stream has been decoded, false otherwise.
            virtual Boole IsEndOfStream() const = 0;

            /// @brief Gets the sample size based on the decoders current configuration.
            /// @return Returns a UInt32 representing the size of a single sample from the underlying stream.
            virtual UInt32 GetSampleSize() const
            {
                switch(this->GetBitConfiguration())
                {
                    case Audio::BC_8Bit_Mono:     return 1;  break;
                    case Audio::BC_8Bit_Stereo:   return 2;  break;
                    case Audio::BC_16Bit_Mono:    return 2;  break;
                    case Audio::BC_16Bit_Stereo:  return 4;  break;
                    case Audio::BC_24Bit_Mono:    return 3;  break;
                    case Audio::BC_24Bit_Stereo:  return 6;  break;
                    default: return -1;
                }
            }

            /// @brief Sets the position (in bytes) of the stream.
            /// @param Position The number of bytes to move(if relative) or the actual position in the stream to set.
            /// @param Relative Whether or not to move from the current position.  If false this will set from the beginning.
            /// @return Returns true if the position was successfully set, false otherwise.
            virtual Boole SetPosition(Int32 Position, const Boole Relative) = 0;
            /// @brief Gets the current stream position of this decoder.
            /// @return Returns the byte position of this decoder in the stream.
            virtual Int32 GetPosition() const = 0;
            /// @brief Moves the current time position in the stream.
            /// @param Seconds The position in seconds to move to in the stream.
            /// @param Relative Whether or not to move from the current position.  If false this will seek from the beginning.
            /// @return Returns true if the position was successfully moved, false otherwise.
            virtual Boole Seek(const Real Seconds, const Boole Relative) = 0;

            /// @brief Reads from the audio stream and writes what is read to a buffer.
            /// @param Output The buffer to write to when reading the audio stream.
            /// @param Amount The number of bytes desired to be read from the audio stream.
            /// @return Returns the number of bytes successfully read from the audio stream.
            virtual UInt32 ReadAudioData(void* Output, UInt32 Amount) = 0;

            ///////////////////////////////////////////////////////////////////////////////
            // Stream Stats

            /// @brief Gets the length of the stream in seconds.
            /// @return Returns the total amount of time needed to playback the sound in seconds.
            virtual Real GetTotalTime() const = 0;
            /// @brief Gets the current time position in the stream.
            /// @return Returns the current position in the stream in seconds.
            virtual Real GetCurrentTime() const = 0;
            /// @brief Gets the size of the decoded audio source in use.
            /// @return Returns the size of the decoded audio source.
            virtual UInt32 GetTotalSize() const = 0;
            /// @brief Gets the size of the encoded audio source in use.
            /// @return Returns the size of the encoded audio source.
            virtual UInt32 GetCompressedSize() const = 0;
            /// @brief Gets the sounds current position in the decoded audio source.
            /// @return Returns the current position in the decoded audio source in bytes.
            virtual UInt32 GetCurrentPosition() const = 0;
            /// @brief Gets the sounds current position in the encoded audio source.
            /// @return Returns the current position in the encoded audio source in bytes.
            virtual UInt32 GetCurrentCompressedPosition() const = 0;
        };//iDecoder
    }//Audio
}//Mezzanine

#endif
