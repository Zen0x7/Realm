#pragma once

#include <boost/foreach.hpp>

namespace boost
{
    namespace BOOST_FOREACH = foreach;
}

#define foreach BOOST_FOREACH