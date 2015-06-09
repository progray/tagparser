#include "abstracttrack.h"
#include "exceptions.h"
#include "mediaformat.h"

using namespace std;
using namespace ConversionUtilities;
using namespace ChronoUtilities;
using namespace IoUtilities;

namespace Media {

/*!
 * \class Media::AbstractTrack
 * \brief The AbstractTrack class parses and stores technical information about
 *        video, audio and other kinds of media tracks.
 *
 * The tag class only provides the interface and common functionality. It is meant to be subclassed.
 */

/*!
 * \brief Constructs a new track.
 * \param inputStream Specifies the stream the track will read from to perform particular operations such
 *                    as reading header information.
 * \param outputStream Specifies the stream the track will write to to perform particular operations such
 *                     as updating or making header information.
 * \param startOffset The start offset of the track in the specified \a stream.
 */
AbstractTrack::AbstractTrack(istream &inputStream, ostream &outputStream, uint64 startOffset) :
    m_istream(&inputStream),
    m_ostream(&outputStream),
    m_reader(BinaryReader(&inputStream)),
    m_writer(BinaryWriter(&outputStream)),
    m_startOffset(startOffset),
    m_headerValid(false),
    m_format(),
    m_mediaType(MediaType::Unknown),
    m_version(0.0),
    m_size(0),
    m_trackNumber(0),
    m_id(0),
    m_bitrate(0.0),
    m_maxBitrate(0.0),
    m_sampleRate(0),
    m_extensionSampleRate(0),
    m_bitsPerSample(0),
    m_bytesPerSecond(0),
    m_channelCount(0),
    m_sampleCount(0),
    m_quality(0),
    m_depth(0),
    m_fps(0),
    m_interlaced(false),
    m_timeScale(0),
    m_enabled(true),
    m_default(false),
    m_forced(false),
    m_lacing(false),
    m_encrypted(false),
    m_usedInPresentation(true),
    m_usedWhenPreviewing(true),
    m_colorSpace(0)
{}

/*!
 * \brief Constructs a new track.
 * \param stream Specifies the stream the track will read from or write
 *               to to perform particular operations such as reading header
 *               information.
 * \param startOffset The start offset of the track in the specified \a stream.
 */
AbstractTrack::AbstractTrack(iostream &stream, uint64 startOffset) :
    AbstractTrack(stream, stream, startOffset)
{}

/*!
 * \brief Destroys the track.
 */
AbstractTrack::~AbstractTrack()
{}

/*!
 * \brief Returns the format of the track as C-style string if known; otherwise
 *        returns the format abbreviation or an empty string.
 * \remarks
 *  - The caller must not free the returned string.
 *  - The string might get invalidated when the track is (re)parsed.
 */
const char *AbstractTrack::formatName() const
{
    return m_format || m_formatName.empty() ? m_format.name() : m_formatName.c_str();
}

/*!
 * \brief Returns the a more or less common abbreviation for the format of the track
 *        as C-style string if known; otherwise returns an empty string.
 */
const char *AbstractTrack::formatAbbreviation() const
{
    const char *abbr = m_format.abbreviation();
    return *abbr || m_formatId.empty() ? m_format.abbreviation() : m_formatId.c_str();
}

/*!
 * \brief Returns the media type as string.
 */
const char *AbstractTrack::mediaTypeName() const
{
    switch(m_mediaType) {
    case MediaType::Audio:
        return "Audio";
    case MediaType::Video:
        return "Video";
    case MediaType::Text:
        return "Subititle";
    case MediaType::Hint:
        return "Hint";
    case MediaType::Unknown:
        return "Other";
    default:
        return "";
    }
}

/*!
 * \brief Returns a label for the track.
 */
string AbstractTrack::label() const
{
    stringstream ss;
    ss << "ID: " << id();
    ss << ", type: " << mediaTypeName();
    if(!name().empty()) {
        ss << ", name: \"" << name() << "\"";
    }
    if(!language().empty() && language() != "und") {
        ss << ", language: \"" << language() << "\"";
    }
    return ss.str();
}

/*!
 * \brief Parses technical information about the track from the header.
 *
 * The information will be read from the associated stream. The stream and the
 * start offset of the track have been specified when constructing the Track.
 *
 * The parsed information can be accessed using the corresponding methods.
 *
 * \throws Throws std::ios_base::failure when an IO error occurs.
 * \throws Throws Media::Failure or a derived exception when a parsing
 *         error occurs.
 */
void AbstractTrack::parseHeader()
{
    invalidateStatus();
    m_headerValid = false;
    m_istream->seekg(m_startOffset, ios_base::beg);
    try {
        internalParseHeader();
        m_headerValid = true;
    } catch(Failure &) {
        throw;
    }
}

/*!
 * \fn AbstractTrack::internalParseHeader()
 * \brief This method is internally called to parse header information.
 *        It needs to be implemented when subclassing this class.
 *
 * \throws Throws std::ios_base::failure when an IO error occurs.
 * \throws Throws Media::Failure or a derived exception when a parsing
 *         error occurs.
 */

}
