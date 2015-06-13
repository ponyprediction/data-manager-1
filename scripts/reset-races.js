db = connect("ponyprediction");
db.races.drop();
db.races.createIndex({"id":1}, {unique:true});
