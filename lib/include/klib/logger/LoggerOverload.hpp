#ifndef LOGGEROVERLOAD_HPP_
# define LOGGEROVERLOAD_HPP_

# include "klib/Traits.hpp"
# include "klib/Utils.hpp"

# include "LoggerUtils.hpp"
# include "Logger.hpp"

namespace KLib
{
template <typename DumpableObject>
static typename Traits::enable_if_c<detail::has_dump_method<DumpableObject>::value, Logger&>::type
operator<<(Logger& logger, const DumpableObject& object)
{
    object.dump(logger);
    return logger;
}

namespace detail
{
template <typename Data>
struct DataPrinter
{
    using DumperType = Dumper<Data>;
    typedef typename Traits::static_if_c<detail::HasDoubleRedirectionOperator<Logger, Data>::value, const Data&, DumperType>::type return_type;

    template <typename DataType, typename... Args>
    static inline typename Traits::enable_if_c<detail::HasDoubleRedirectionOperator<Logger, DataType>::value, const DataType&>::type
    data_to_log(DataType const& data, Args&&...)
    {
        return data;
    }

    template <typename DataType, typename... Args>
    static inline typename Traits::enable_if_c<!detail::HasDoubleRedirectionOperator<Logger, DataType>::value, DumperType>::type
    data_to_log(DataType const& data, Args&&... args)
    {
        return DumperType(data, args...);
    }

};
}

Logger& endl(Logger& logger);
Logger& operator<<(Logger& logger, const char* str);
Logger& operator<<(Logger& logger, Logger& (*func)(Logger&));

# define GENERATE_DUMPER(type)                      \
        Logger& operator<<(Logger& logger, type i);

GENERATE_DUMPER(int);
GENERATE_DUMPER(unsigned int);
GENERATE_DUMPER(char);
GENERATE_DUMPER(unsigned char);
GENERATE_DUMPER(short);
GENERATE_DUMPER(unsigned short);
#undef GENERATE_DUMPER

}

template <typename Obj, typename... Args>
static typename KLib::detail::DataPrinter<Obj>::return_type dump_data(const Obj& obj, Args&&... args)
{
    return KLib::detail::DataPrinter<Obj>::data_to_log(obj, args...);
}

#endif /* LOGGEROVERLOAD_HPP_ */
