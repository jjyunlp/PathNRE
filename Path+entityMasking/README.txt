This is the README.
This is a first version of entity masking based path 推理
以关系为中心聚集所有类别，并且每一个关系下的实体对被统一的vector替换
1)Each entity in sentence are replaced by a universal embedding.
In this way, we can enhance the connections between different sentences under the same relation that they represented.

For path calculation in test sentences, I am try to find the path without the entity pair.

The problem for entity pair based path is only a small part of the sentences can find a possible path for current e1 and e2:
like <e1,e2> in the test sentence, we treasure all sentences to find <e1, e*> + <e*, e2>.

So, for a test sentence, I hope the model can predicate top-N relations.
Based on the top-N relations, we can give some possible paths based on a relation model.

For the relation model, I will build it based on the entities in training data.
This is not a paradox.
For the training data, though few sentences can been find a path based on entities, it is possible for building a relation model.
Like {R1=R2+R3, R2=R3+R5,...}
During testing, if we did it based on entities, few sentences will been got information from the path.
However, if we can give path information based on top-N relations.


Each folder contains codes for a model. To run the code, you need:

1)Download the dataset, and put it in to the folder “data”.
2)Access to a folder.
3)Run this code in your terminal: g++ train_cnn.cpp -o Train -pthread -O3
4)Run the file “Train”.
5)The model will be stored in “./out” folder.

To test the model, you need:
1)Access to the same folder.
2)Run this code: g++ work.cpp -o Work -pthread -O3
3)Run the file “Work”.
4)The result will be recorded in out/pr.txt
