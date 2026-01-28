#pragma once

#include <koo/Export.hpp>
#include <koo/dyna/Keyword.hpp>
#include <koo/util/Types.hpp>
#include <vector>
#include <string>

namespace koo::dyna {

/**
 * @brief Base class for *FREQUENCY keywords
 */
class KOO_API FrequencyKeyword : public Keyword {
public:
    virtual ~FrequencyKeyword() = default;
};

/**
 * @brief *FREQUENCY_DOMAIN_ACOUSTIC_FEM
 *
 * Frequency domain acoustic analysis using FEM.
 */
class KOO_API FrequencyDomainAcousticFem : public CloneableKeyword<FrequencyDomainAcousticFem, FrequencyKeyword> {
public:
    struct Data {
        int nfreq = 0;        // Number of frequencies
        double fmin = 0.0;    // Minimum frequency
        double fmax = 0.0;    // Maximum frequency
        int mdmin = 0;        // Minimum mode number
        int mdmax = 0;        // Maximum mode number
        double dampf = 0.0;   // Modal damping factor
        int lcdam = 0;        // Damping curve
        int lctyp = 0;        // Damping curve type
    };

    FrequencyDomainAcousticFem() = default;

    std::string getKeywordName() const override { return "*FREQUENCY_DOMAIN_ACOUSTIC_FEM"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *FREQUENCY_DOMAIN_FRF
 *
 * Frequency Response Function computation.
 */
class KOO_API FrequencyDomainFrf : public CloneableKeyword<FrequencyDomainFrf, FrequencyKeyword> {
public:
    struct Data {
        int n1 = 0;           // First node
        int n2 = 0;           // Second node (optional)
        int dession = 0;      // Excitation DOF
        int ression = 0;      // Response DOF
        double dampf = 0.0;   // Modal damping
        int mdmin = 0;        // Minimum mode
        int mdmax = 0;        // Maximum mode
        int fnmin = 0;        // Minimum frequency number
        int fnmax = 0;        // Maximum frequency number
        double fmin = 0.0;    // Minimum frequency
        double fmax = 0.0;    // Maximum frequency
        int nfreq = 0;        // Number of frequencies
        int lcdam = 0;        // Damping curve
    };

    FrequencyDomainFrf() = default;

    std::string getKeywordName() const override { return "*FREQUENCY_DOMAIN_FRF"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *FREQUENCY_DOMAIN_RANDOM_VIBRATION
 *
 * Random vibration analysis in frequency domain.
 */
class KOO_API FrequencyDomainRandomVibration : public CloneableKeyword<FrequencyDomainRandomVibration, FrequencyKeyword> {
public:
    struct Data {
        int ldpsd = 0;        // Load PSD curve
        int vession = 0;      // Vibration excitation node set
        int respid = 0;       // Response node set
        int mdmin = 0;        // Minimum mode
        int mdmax = 0;        // Maximum mode
        double dampf = 0.0;   // Modal damping factor
        int lcdam = 0;        // Damping curve
        double fmin = 0.0;    // Minimum frequency
        double fmax = 0.0;    // Maximum frequency
        int nfreq = 0;        // Number of frequencies
    };

    FrequencyDomainRandomVibration() = default;

    std::string getKeywordName() const override { return "*FREQUENCY_DOMAIN_RANDOM_VIBRATION"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *FREQUENCY_DOMAIN_SSD
 *
 * Steady state dynamics in frequency domain.
 */
class KOO_API FrequencyDomainSsd : public CloneableKeyword<FrequencyDomainSsd, FrequencyKeyword> {
public:
    struct Data {
        int mdmin = 0;        // Minimum mode number
        int mdmax = 0;        // Maximum mode number
        double dampf = 0.0;   // Modal damping factor
        int lcdam = 0;        // Damping curve
        int lctyp = 0;        // Damping type
        double fmin = 0.0;    // Minimum frequency
        double fmax = 0.0;    // Maximum frequency
        int nfreq = 0;        // Number of frequencies
        int restrt = 0;       // Restart flag
    };

    FrequencyDomainSsd() = default;

    std::string getKeywordName() const override { return "*FREQUENCY_DOMAIN_SSD"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *FREQUENCY_DOMAIN_ACOUSTIC_BEM
 *
 * Frequency domain acoustic analysis using BEM.
 */
class KOO_API FrequencyDomainAcousticBem : public CloneableKeyword<FrequencyDomainAcousticBem, FrequencyKeyword> {
public:
    struct Data {
        int nfreq = 0;        // Number of frequencies
        double fmin = 0.0;    // Minimum frequency
        double fmax = 0.0;    // Maximum frequency
        int ssid = 0;         // Segment set ID
        double rho = 0.0;     // Fluid density
        double bulk = 0.0;    // Bulk modulus
    };

    FrequencyDomainAcousticBem() = default;

    std::string getKeywordName() const override { return "*FREQUENCY_DOMAIN_ACOUSTIC_BEM"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *FREQUENCY_DOMAIN_MODE
 *
 * Frequency domain mode selection.
 */
class KOO_API FrequencyDomainMode : public CloneableKeyword<FrequencyDomainMode, FrequencyKeyword> {
public:
    struct Data {
        int mdmin = 0;        // Minimum mode
        int mdmax = 0;        // Maximum mode
        int fnmin = 0;        // Frequency min number
        int fnmax = 0;        // Frequency max number
        double fmin = 0.0;    // Minimum frequency
        double fmax = 0.0;    // Maximum frequency
    };

    FrequencyDomainMode() = default;

    std::string getKeywordName() const override { return "*FREQUENCY_DOMAIN_MODE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *FREQUENCY_DOMAIN_PATH
 *
 * Frequency domain path definition.
 */
class KOO_API FrequencyDomainPath : public CloneableKeyword<FrequencyDomainPath, FrequencyKeyword> {
public:
    struct Data {
        int id = 0;           // Path ID
        int n1 = 0;           // Start node
        int n2 = 0;           // End node
        int npts = 0;         // Number of points
        int type = 0;         // Path type
    };

    FrequencyDomainPath() = default;

    std::string getKeywordName() const override { return "*FREQUENCY_DOMAIN_PATH"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *FREQUENCY_DOMAIN_RESPONSE_SPECTRUM
 *
 * Response spectrum analysis in frequency domain.
 */
class KOO_API FrequencyDomainResponseSpectrum : public CloneableKeyword<FrequencyDomainResponseSpectrum, FrequencyKeyword> {
public:
    struct Data {
        int lcid = 0;         // Load curve ID
        int mdmin = 0;        // Minimum mode
        int mdmax = 0;        // Maximum mode
        double dampf = 0.0;   // Modal damping
        int dession = 0;      // Excitation direction
        int comb = 0;         // Combination method
    };

    FrequencyDomainResponseSpectrum() = default;

    std::string getKeywordName() const override { return "*FREQUENCY_DOMAIN_RESPONSE_SPECTRUM"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *FREQUENCY_DOMAIN_SSD_DIRECT
 *
 * Direct steady state dynamics in frequency domain.
 */
class KOO_API FrequencyDomainSsdDirect : public CloneableKeyword<FrequencyDomainSsdDirect, FrequencyKeyword> {
public:
    struct Data {
        double fmin = 0.0;    // Minimum frequency
        double fmax = 0.0;    // Maximum frequency
        int nfreq = 0;        // Number of frequencies
        double dampf = 0.0;   // Modal damping factor
        int lcdam = 0;        // Damping curve
    };

    FrequencyDomainSsdDirect() = default;

    std::string getKeywordName() const override { return "*FREQUENCY_DOMAIN_SSD_DIRECT"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

/**
 * @brief *FREQUENCY_DOMAIN_VIBRATION_FATIGUE
 *
 * Vibration fatigue analysis in frequency domain.
 */
class KOO_API FrequencyDomainVibrationFatigue : public CloneableKeyword<FrequencyDomainVibrationFatigue, FrequencyKeyword> {
public:
    struct Data {
        int ldpsd = 0;        // Load PSD curve
        int respid = 0;       // Response node set
        int mdmin = 0;        // Minimum mode
        int mdmax = 0;        // Maximum mode
        double dampf = 0.0;   // Modal damping
        double sth = 0.0;     // Stress threshold
    };

    FrequencyDomainVibrationFatigue() = default;

    std::string getKeywordName() const override { return "*FREQUENCY_DOMAIN_VIBRATION_FATIGUE"; }

    bool parse(const std::vector<std::string>& lines,
               util::CardParser::Format format = util::CardParser::Format::Standard) override;

    std::vector<std::string> write(
        util::CardParser::Format format = util::CardParser::Format::Standard) const override;

    void accept(ModelVisitor& visitor) override;

    Data& getData() { return data_; }
    const Data& getData() const { return data_; }

private:
    Data data_;
};

} // namespace koo::dyna
