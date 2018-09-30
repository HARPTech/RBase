#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>

#include "../include/RUtil/Logging.hpp"

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

BOOST_LOG_ATTRIBUTE_KEYWORD(lrt_logger_severity,
                            "Severity",
                            lrt::rutil::Logging::Severity)
BOOST_LOG_ATTRIBUTE_KEYWORD(lrt_logger_file, "File", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(lrt_logger_function, "Function", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(lrt_logger_line, "Line", int)
BOOST_LOG_ATTRIBUTE_KEYWORD(lrt_logger_timestamp,
                            "Timestamp",
                            boost::posix_time::ptime)

namespace lrt {
namespace rutil {

boost::log::sources::severity_logger<Logging::Severity> Logging::m_mainLogger;
boost::log::sources::severity_logger<Logging::Severity> Logging::m_rcommLogger;
bool Logging::m_initialised = false;

Logging::Logging()
  : m_logDir("")
  , m_mainLogPath("rmaster.log")
{}
Logging::~Logging() {}

boost::log::sources::severity_logger<Logging::Severity>&
Logging::mainLogger()
{
  assert(m_initialised);
  return m_mainLogger;
}
boost::log::sources::severity_logger<Logging::Severity>&
Logging::rcommLogger()
{
  assert(m_initialised);
  return m_rcommLogger;
}

void
Logging::setConsoleOut(bool enabled)
{
  m_consoleOut = enabled;
}

void
Logging::init(const std::string& logDir, bool fileOut, bool consoleOut)
{
  m_logDir = logDir;
  m_mainLogPath = logDir + "/rmaster.log";
  m_fileOut = fileOut;
  m_consoleOut = consoleOut;
  m_initialised = true;

  logging::core::get()->add_global_attribute(
    "Line", logging::attributes::mutable_constant<int>(5));
  logging::core::get()->add_global_attribute(
    "File", logging::attributes::mutable_constant<std::string>(""));
  logging::core::get()->add_global_attribute(
    "Function", logging::attributes::mutable_constant<std::string>(""));
  logging::core::get()->add_global_attribute(
    "Timestamp", logging::attributes::local_clock());
  logging::add_common_attributes();

  if(m_fileOut) {
    logging::add_file_log(m_mainLogPath,
                          keywords::format =
                            (expr::stream << "[" << lrt_logger_timestamp << "] "
                                          << "[" << lrt_logger_severity << "] "
                                          << "(" << lrt_logger_file << ":"
                                          << lrt_logger_line << ") "
                                          << "(" << lrt_logger_function
                                          << "): " << expr::smessage));
  }

  if(m_consoleOut) {
    auto consoleSink = logging::add_console_log(
      std::clog,
      keywords::format =
        (expr::stream << "[" << lrt_logger_timestamp << "] ["
                      << expr::attr<Logging::Severity, severity_tag>("Severity")
                      << "] " << expr::smessage));

    logging::core::get()->add_sink(consoleSink);
  }

  if(m_consoleOut) {
    LRT_MAIN_LOG(Debug) << "Initialized Logging.";
  }
}

std::ostream&
operator<<(std::ostream& strm, Logging::Severity level)
{
  static const char* strings[] = { "Trace", "Debug", "Info",    "Warning",
                                   "Alert", "Error", "Failure", "Fatal" };
  if(static_cast<std::size_t>(level) < sizeof(strings) / sizeof(*strings))
    strm << strings[level];
  else
    strm << static_cast<int>(level);

  return strm;
}
boost::log::formatting_ostream&
operator<<(
  boost::log::formatting_ostream& strm,
  boost::log::to_log_manip<Logging::Severity, severity_tag> const& manip)
{
  static const char* colorised_strings[] = {
    "\033[30mTRCE\033[0m", "\033[32mDEBG\033[0m", "\033[37mINFO\033[0m",
    "\033[33mWARN\033[0m", "\033[33mALRT\033[0m", "\033[31mERRR\033[0m",
    "\033[31mFAIL\033[0m", "\033[31mFTAL\033[0m"
  };
  static const char* uncolorised_strings[] = { "TRCE", "DEBG", "INFO", "WARN",
                                               "ALRT", "ERRR", "FAIL", "FTAL" };

  static const char** strings = nullptr;

  if(strings == nullptr) {
    std::string term = std::getenv("TERM");
    if(term.find("color") != std::string::npos) {
      strings = colorised_strings;
    } else {
      strings = uncolorised_strings;
    }
  }

  Logging::Severity level = manip.get();

  if(static_cast<std::size_t>(level) <
     sizeof(colorised_strings) / sizeof(*colorised_strings))
    strm << strings[level];
  else
    strm << static_cast<int>(level);

  return strm;
}
}
}
