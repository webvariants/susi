#include "db/Database.h"

Poco::Dynamic::Var Susi::DB::Database::query(std::string query){
	soci::rowset<soci::row> rows = (session.prepare << query);
	Poco::Dynamic::Vector results;
	for(soci::row & row : rows){
		Poco::Dynamic::Struct<std::string> entry;
		for(std::size_t i = 0; i != row.size(); ++i){
			const soci::column_properties & props = row.get_properties(i);
			std::string name = props.get_name();
			Poco::Dynamic::Var value;
			switch(props.get_data_type()){
				case soci::dt_string:
					value = row.get<std::string>(i);
					break;
				case soci::dt_double:
					value = row.get<double>(i);
					break;
				case soci::dt_integer:
					value = row.get<int>(i);
					break;
				case soci::dt_long_long:
					value = row.get<long long>(i);
					break;
				case soci::dt_unsigned_long_long:
					value = row.get<unsigned long long>(i);
					break;
				case soci::dt_date:
					std::tm when = row.get<std::tm>(i);
					value = asctime(&when);
					break;
			}
			entry[name] = value;
		}
		results.push_back(entry);
	}
	return results;
}