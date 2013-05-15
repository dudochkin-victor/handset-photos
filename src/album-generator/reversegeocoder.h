#ifndef REVERSE_GEO_CODER
#define REVERSE_GEO_CODER

// interface that will be called when 
// a lat/long has been reverse geo coded
struct ReverseGeocoderAction {
    virtual void onTranslated(const QVariant &userdata, const QString &location) = 0;
};

class ReverseGeocoder {
public:
    
    virtual void reverseGeocoding(const QVariant &userdata, float lat, float lon) = 0;
};

#endif
