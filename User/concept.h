#ifndef __USER_CONCEPT_H__
#define __USER_CONCEPT_H__

using Json = nlohmann::json;

template<typename T>
concept ToJsonAble = requires (T t, Json json) {to_json(json, t);};

template<typename T>
concept FromJsonAble = requires (T t, Json json) {from_json(json, t);};

template<typename T>
concept TurnJsonAble = (ToJsonAble<T> && FromJsonAble<T>) ;

#endif