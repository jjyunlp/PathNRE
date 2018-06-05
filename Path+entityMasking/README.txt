This is the README.
This is a first version of entity masking based path 推理
以关系为中心聚集所有类别，并且每一个关系下的实体对被统一的vector替换
1)Each entity in sentence are replaced by a universal embedding.
In this way, we can enhance the connections between different sentences under the same relation that they represented.


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
