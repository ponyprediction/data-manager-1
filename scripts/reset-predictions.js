db = connect("ponyprediction");
db.predictions.drop();
db.predictions.createIndex({"id":1}, {unique:true});
