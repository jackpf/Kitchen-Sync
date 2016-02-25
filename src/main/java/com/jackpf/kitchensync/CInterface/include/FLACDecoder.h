#ifndef __Kitchen_Sync_FLAC_Decoder_h__
#define __Kitchen_Sync_FLAC_Decoder_h__

class FLACDecoder : public FLAC::Decoder::File {
private:
	FLACDecoder(const FLACDecoder&);
	FLACDecoder&operator=(const FLACDecoder&);

protected:
	FILE *f;

	virtual ::FLAC__StreamDecoderWriteStatus write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[]);
	virtual void metadata_callback(const ::FLAC__StreamMetadata *metadata);
	virtual void error_callback(::FLAC__StreamDecoderErrorStatus status);

public:
	FLACDecoder(FILE *f_): FLAC::Decoder::File(), f(f_) { }
};

#endif