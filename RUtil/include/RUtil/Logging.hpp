#ifndef LRT_RSUPPORT_LOGGING_HPP
#define LRT_RSUPPORT_LOGGING_HPP

#include <boost/log/attributes/mutable_constant.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/utility/formatting_ostream.hpp>
#include <boost/log/utility/manipulators/to_log.hpp>
#include <string>

#ifndef FILE_BASENAME
#define FILE_BASENAME "Basename not supported."
#endif

#define LRT_LOGGING_LOCATION(lg)                                             \
  boost::log::attribute_cast<boost::log::attributes::mutable_constant<int>>( \
    boost::log::core::get()->get_global_attributes()["Line"])                \
    .set(__LINE__);                                                          \
  boost::log::attribute_cast<                                                \
    boost::log::attributes::mutable_constant<std::string>>(                  \
    boost::log::core::get()->get_global_attributes()["File"])                \
    .set(FILE_BASENAME);                                                     \
  boost::log::attribute_cast<                                                \
    boost::log::attributes::mutable_constant<std::string>>(                  \
    boost::log::core::get()->get_global_attributes()["Function"])            \
    .set(__PRETTY_FUNCTION__);

#define LRT_MAIN_LOG(severity) \
  LRT_SEV_LOG(lrt::rutil::Logging::mainLogger(), severity)

#define LRT_RCOMM_LOG(severity) \
  LRT_SEV_LOG(lrt::rutil::Logging::rcommLogger(), severity)

#define LRT_SEV_LOG(lg, severity) \
  do {                            \
    LRT_LOGGING_LOCATION(lg)      \
  } while(false);                 \
  BOOST_LOG_SEV(lg, ::lrt::rutil::Logging::Severity::severity)

namespace lrt {
namespace rutil {

struct severity_tag;

class Logging
{
  public:
  Logging();
  virtual ~Logging();

  void init(const std::string& logDir = "./logs/",
            bool fileOut = true,
            bool consoleOut = true);

  enum Severity
  {
    Trace,
    Debug,
    Info,
    Warning,
    Alert,
    Error,
    Failure,
    Fatal
  };

  static boost::log::sources::severity_logger<Severity>& mainLogger();
  static boost::log::sources::severity_logger<Severity>& rcommLogger();

  void setConsoleOut(bool enabled);

  private:
  static bool m_initialised;
  static boost::log::sources::severity_logger<Severity> m_mainLogger;
  static boost::log::sources::severity_logger<Severity> m_rcommLogger;

  std::string m_logDir;

  std::string m_mainLogPath;

  bool m_fileOut = true;
  bool m_consoleOut = true;
};

std::ostream&
operator<<(std::ostream& strm, Logging::Severity level);

boost::log::formatting_ostream&
operator<<(
  boost::log::formatting_ostream& strm,
  boost::log::to_log_manip<Logging::Severity, severity_tag> const& manip);
}
}

#endif
