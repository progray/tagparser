#include "mpegaudioframe.h"
#include "../exceptions.h"

#include <c++utilities/io/binaryreader.h>

using namespace std;
using namespace IoUtilities;

namespace Media {

const uint64 MpegAudioFrame::m_xingHeaderOffset = 0x24;

/*!
 * \class Media::MpegAudioFrame
 * \brief The MpegAudioFrame class is used to parse MPEG audio frames.
 */

const int MpegAudioFrame::m_bitrateTable[0x2][0x3][0xF] =
{
    {{0,32,64,96,128,160,192,224,256,288,320,352,384,416,448},
     {0,32,48,56,64,80,96,112,128,160,192,224,256,320,384},
     {0,32,40,48,56,64,80,96,112,128,160,192,224,256,320}},
    {{0,32,48,56,64,80,96,112,128,144,160,176,192,224,256},
     {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160},
     {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160}}
};

const uint32 MpegAudioFrame::m_sync = 0xFFE00000u;

/*!
 * \brief Parses the header read using the specified \a reader.
 * \throws Throws InvalidDataException if the data read from the stream is
 *         no valid frame header.
 */
void MpegAudioFrame::parseHeader(BinaryReader &reader)
{
    m_header = reader.readUInt32BE();
    reader.stream()->seekg(m_xingHeaderOffset - 4, ios_base::cur);
    m_xingHeader = reader.readUInt64BE();
    m_xingHeaderFlags = static_cast<XingHeaderFlags>(m_xingHeader & 0xffffffffuL);
    if(isXingHeaderAvailable()) {
        if(isXingFramefieldPresent()) {
            m_xingFramefield = reader.readUInt32BE();
        }
        if(isXingBytesfieldPresent()) {
            m_xingBytesfield = reader.readUInt32BE();
        }
        if(isXingTocFieldPresent()) {
            reader.stream()->seekg(64, ios_base::cur);
        }
        if(isXingQualityIndicatorFieldPresent()) {
            m_xingQualityIndicator = reader.readUInt32BE();
        }
    }
    if(!isValid()) {
        throw InvalidDataException();
    }
}

/*!
 * \brief Returns the MPEG version if known (1.0, 2.0 or 2.5); otherwise returns 0.
 */
double MpegAudioFrame::mpegVersion() const
{
    switch(m_header & 0x180000u) {
    case 0x180000u:
        return 1.0;
    case 0x100000u:
        return 2.0;
    case 0x0u:
        return 2.5;
    default:
        return 0.0;
    }
}

/*!
 * \brief Returns the MPEG layer if known (1, 2, or 3); otherwise returns 0.
 */
int MpegAudioFrame::layer() const
{
    switch (m_header & 0x60000u) {
    case 0x60000u:
        return 1;
    case 0x40000u:
        return 2;
    case 0x20000u:
        return 3;
    default:
        return 0;
    }
}

/*!
 * \brief Returns the sampel rate of the frame if known; otherwise returns 0.
 */
uint32 MpegAudioFrame::sampelRate() const
{
    switch (m_header & 0xc00u) {
    case 0xc00u:
        return 0;
    case 0x800u:
        switch (m_header & 0x180000u) {
        case 0x180000u:
            return 32000;
        case 0x100000u:
            return 16000;
        case 0x0u:
            return 8000u;
        }
        break;
    case 0x400u:
        switch (m_header & 0x180000u) {
        case 0x180000u:
            return 48000;
        case 0x100000u:
            return 24000;
        case 0x0u:
            return 12000;
        }
        break;
    case 0x0u:
        switch (m_header & 0x180000u) {
        case 0x180000u:
            return 44100;
        case 0x100000:
            return 22050;
        case 0x0u:
            return 11025;
        }
        break;
    }
    return 0;
}

/*!
 * \brief Returns the channel mode if known; otherwise returns MpegChannelMode::Unspecifed.
 */
MpegChannelMode MpegAudioFrame::channelMode() const
{
    if(isValid()) {
        switch (m_header & 0xc0u) {
        case 0xc0u:
            return MpegChannelMode::SingleChannel;
        case 0x80u:
            return MpegChannelMode::DualChannel;
        case 0x40u:
            return MpegChannelMode::JointStereo;
        case 0x00:
            return MpegChannelMode::Stereo;
        default:
            ;
        }
    }
    return MpegChannelMode::Unspecifed;
}

/*!
 * \brief Returns the sample count if known; otherwise returns 0.
 */
uint32 MpegAudioFrame::sampleCount() const
{
    switch (m_header & 0x60000u) {
    case 0x60000u:
        return 384u;
    case 0x40000u:
        return 1152u;
    case 0x20000u:
        switch (m_header & 0x180000u) {
        case 0x180000u:
            return 1152u;
        case 0x100000u:
        case 0x0u:
            return 576u;
        }
    default:
        ;
    }
    return 0;
}

/*!
 * \brief Returns the size if known; otherwise retruns 0.
 */
uint32 MpegAudioFrame::size() const
{
    switch (m_header & 0x60000u) {
    case 0x60000u:
        return static_cast<uint32>(((static_cast<double>(bitrate()) * 1024.0 / 8.0) / static_cast<double>(sampelRate())) * static_cast<double>(sampleCount()) + static_cast<double>(paddingSize()));
    case 0x40000u:
    case 0x20000u:
        return static_cast<uint32>(((static_cast<double>(bitrate()) * 1024.0 / 8.0) / static_cast<double>(sampelRate())) * static_cast<double>(sampleCount()) + static_cast<double>(paddingSize()));
    default:
        return 0;
    }
}

}
