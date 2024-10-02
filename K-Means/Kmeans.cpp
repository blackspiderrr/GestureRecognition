#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<fstream>
#include<vector>
#include<iostream>

#define MAX_ROUNDS 100    //最大允许的聚类次数

//“点”的结构体  
typedef struct Point{
  // int x_value;           //用于存放点在X轴上的值
  // int y_value;           //用于存放点在Y轴上的值
  double values[16];
  int cluster_id;        //用于存放该点所属的cluster id
}Point;
Point* data;
 
//cluster center的结构体
typedef struct ClusterCenter{
  // double x_value;
  // double y_value;
  double values[16];
  int cluster_id;
}ClusterCenter;
ClusterCenter* cluster_center;

//计算cluster center的结构体
typedef struct CenterCalc{
  // double x_value;
  // double y_value;
  double values[16];
}CenterCalc;
CenterCalc *center_calc;

int is_continue;                               //kmeans 运算是否继续
int* cluster_center_init_index;        //记录每个cluster center最初用的是哪个“点”
double* distance_from_center;      //记录一个“点”到所有cluster center的距离
int* data_size_per_cluster;            //每个cluster点的个数
int data_size_total;                        //设定点的个数
char filename[200];                       //要读取的点的数据的文件名
int cluster_count;                          //设定的cluster的个数
 
void memoryAlloc();
void memoryFree();
void readDataFromFile(std::string filename, std::vector<std::vector<double>>& result);
void initialCluster(std::vector<std::vector<double>>& result);
void calcDistance2OneCenter(int pointID, int centerID);
void calcDistance2AllCenters(int pointID);
void partition4OnePoint(int pointID);
void partition4AllPointOneCluster();
void calcClusterCenter();
void kmeans();
void compareNewOldClusterCenter(CenterCalc* center_calc);
 
int main(int argc, char* argv[])
{
    if( argc != 4 )
    {
        printf("This application needs 3 parameters to run:"
            "\n the 1st is the size of data set,"
            "\n the 2nd is the file name that contains data"
            "\n the 3rd indicates the cluster_count"
            "\n");
        exit(1);
    }

    data_size_total = atoi(argv[1]);
    strcat(filename, argv[2]);
    cluster_count = atoi(argv[3]);
    std::cout<< "start memory alloc";
    //1, memory alloc
    memoryAlloc();
    //2, read point data from file
    std::cout<< "start read data from file"<<std::endl;
    std::vector<std::vector<double>> filedata; // data[i][j] means ith data, jth index
    readDataFromFile(filename, filedata);
    //3, initial cluster
    initialCluster(filedata);
    
    for (int i=0; i < 200;i++) {
      for (int j=0; j< 16; j++){
        data[i].values[j] = filedata[i][j];
        data[i].cluster_id = i/50;
      }
    }

    //4, run k-means
    kmeans();
    //5, memory free & end
    memoryFree();
    
    return 0;
}

void memoryAlloc()
{
  data = (Point*)malloc(sizeof(struct Point) * (data_size_total));
  if( !data )
  {
    printf("malloc error:data!");
    exit(1);
  }
  cluster_center_init_index = (int*)malloc(sizeof(int) * (cluster_count));
  if( !cluster_center_init_index )
  {
    printf("malloc error:cluster_center!\n");
    exit(1);
  }
  distance_from_center = (double*)malloc(sizeof(double) * (cluster_count));
  if( !distance_from_center )
  {
    printf("malloc error: distance_from_center!\n");
    exit(1);
  }
  cluster_center = (ClusterCenter*)malloc(sizeof(struct ClusterCenter) * (cluster_count));
  if( !cluster_center )
  {
    printf("malloc cluster center new error!\n");
    exit(1);
  }

  center_calc = (CenterCalc*)malloc(sizeof(CenterCalc) * cluster_count);
  if( !center_calc )
  {
    printf("malloc error: center_calc!\n");
    exit(1);
  }

  data_size_per_cluster = (int*)malloc(sizeof(int) * (cluster_count));
  if( !data_size_per_cluster )
  {
    printf("malloc error: data_size_per_cluster\n");
    exit(1);
  }
 
}

void memoryFree()
{
  free(data);
  data = NULL;
  free(cluster_center_init_index);
  cluster_center_init_index = NULL;
  free(distance_from_center);
  distance_from_center = NULL;
  free(cluster_center);
  cluster_center = NULL;
  free(center_calc);
  center_calc = NULL;
  free(data_size_per_cluster);
  data_size_per_cluster = NULL;
}

//从文件中读入每个点的值
void readDataFromFile(std::string filename, std::vector<std::vector<double>>& result)
{
  std::ifstream inputFile(filename);
  if (inputFile.is_open()) {
    std::string line;
    int i = 0;
    std::vector<double> data;
    while (std::getline(inputFile, line)) {
      size_t pos = line.find("=");
      if (pos == std::string::npos) {
        continue;
      }
      double value = std::stod(line.substr(pos + 1));
      data.push_back(value);
      i += 1;
      if (i==16) {
        i = 0;
        result.push_back(data);
        data.clear();
      }
    }
    inputFile.close();

    // test
    for (int i=0;i < result.size();i++) {
      // std::cout<<"new data:";
      for (int j=0; j< result[i].size(); j++) {
        std::cout << result[i][j] << ',';
      }
      std::cout << std::endl;
    }
    std::cout<<std::endl;
  }
}

//根据传入的cluster_count来随机的选择一个点作为 一个cluster的center  
void initialCluster(std::vector<std::vector<double>>& result)
{
  int i, j, k;
  // int random;
  // //产生初始化的cluster_count个聚类  
  // for( i = 0; i < cluster_count; i++ ){
  //   cluster_center_init_index[i] = -1;
  // }
  // //随机选择一个点作为每个cluster的center（不重复）
  // for( i = 0; i < cluster_count; i++ ){
  //   Reselect:
  //       random = rand() % (data_size_total - 1);
  //       for(j = 0; j < i; j++) {
  //           if(random == cluster_center_init_index[j])
  //               goto Reselect;
  //       }
  //   cluster_center_init_index[i] = random;
  //   printf("cluster_id: %d, located in point index:%d\n", i, random);  
  // }

  std::vector<double> matrix0 = {3.139, 3.123, 3.102, 3.130, 3.160, 3.135, 3.130, 3.139, 3.147, 2.960, 2.868, 3.133, 3.157, 2.483, 2.350, 3.127};
  std::vector<double> matrix1 = {3.146, 3.135, 3.150, 3.133, 3.157, 1.167, 3.029, 3.140, 3.146, 2.220, 2.053, 3.114, 3.148, 2.934, 2.089, 3.115};
  std::vector<double> matrix2 = {3.152, 3.077, 1.948, 3.117, 3.170, 2.606, 1.520, 3.006, 3.150, 3.135, 1.690, 3.119, 3.162, 1.531, 1.981, 1.800};
  std::vector<double> matrix3 = {3.146, 3.133, 3.148, 3.132, 3.158, 3.149, 3.143, 3.141, 3.146, 3.127, 3.143, 3.142, 3.156, 3.156, 3.142, 3.144};
  
  //将随机选择的点作为center，同时这个点的cluster id也就确定了
  for( i = 0; i < cluster_count; i++ ){
    // cluster_center[i].x_value = data[cluster_center_init_index[i]].x_value;
    // cluster_center[i].y_value = data[cluster_center_init_index[i]].y_value;
    // for( k = 0; k < 16; k++){
    //    cluster_center[i].values[k] = result[cluster_center_init_index[i]][k];
    // }
    for( k = 0; k < 16; k++){
      cluster_center[0].values[k] = matrix0[k];
      cluster_center[1].values[k] = matrix1[k];
      cluster_center[2].values[k] = matrix2[k];
      cluster_center[3].values[k] = matrix3[k];
    }
    cluster_center[i].cluster_id = i;
    //data[cluster_center_init_index[i]].cluster_id = i;

    //printf("cluster_id:%d, index:%d, x_value:%f, y_value:%f\n", cluster_center[i].cluster_id, cluster_center_init_index[i], cluster_center[i].x_value, cluster_center[i].y_value);
    printf("cluster_id: %d, index:%d\n", cluster_center[i].cluster_id, 0);
    printf("values: ");
    for (k = 0; k < 16; k++) {
      printf("%f,", cluster_center[i].values[k]);
    }
    printf("\n");
  }
}
 
// //计算一个点到一个cluster center的distance
// void calcDistance2OneCenter(int point_id,int center_id)
// {
//   distance_from_center[center_id] = sqrt( (data[point_id].x_value-cluster_center[center_id].x_value)*(double)(data[point_id].x_value-cluster_center[center_id].x_value) + (double)(data[point_id].y_value-cluster_center[center_id].y_value) * (data[point_id].y_value-cluster_center[center_id].y_value) );
// }

void calcDistance2OneCenter(int point_id, int center_id)
{
    double sum = 0.0;
    
    // 计算十六维向量的欧几里得距离
    for (int i = 0; i < 16; i++) {
      double diff = data[point_id].values[i] - cluster_center[center_id].values[i];
      sum += diff * diff;
    }
    
    distance_from_center[center_id] = sqrt(sum);
}

 
//计算一个点到每个cluster center的distance
void calcDistance2AllCenters(int point_id)
{
  int i;
  for( i = 0; i < cluster_count; i++ )
  {
    calcDistance2OneCenter(point_id, i);
  }
}
 
//确定一个点属于哪一个cluster center(取距离最小的)
void partition4OnePoint(int point_id)
{
  int i;
  int min_index = 0;
  double min_value = distance_from_center[0];
  for( i = 0; i < cluster_count; i++ )
  {
    if( distance_from_center[i] < min_value )
    {
      min_value = distance_from_center[i];
      min_index = i;
    }
  }
 
  data[point_id].cluster_id = cluster_center[min_index].cluster_id;
}

//在一轮的聚类中得到所有的point所属于的cluster center
void partition4AllPointOneCluster()
{
  int i;
  for( i = 0; i < data_size_total; i++ )
  {
    if( data[i].cluster_id != -1 )  //这个点就是center，不需要计算
      continue;
    else
    {
      calcDistance2AllCenters(i);  //计算第i个点到所有center的distance
      partition4OnePoint(i);          //根据distance对第i个点进行partition
    }
  }
}

//重新计算新的cluster center
void calcClusterCenter()
{
  int i, j;

  memset(center_calc, 0, sizeof(CenterCalc) * cluster_count);
  memset(data_size_per_cluster, 0, sizeof(int) * cluster_count);
  //分别对每个cluster内的每个点的X和Y求和，并计每个cluster内点的个数
  for( i = 0; i < data_size_total; i++ )
  {
    // center_calc[data[i].cluster_id].x_value += data[i].x_value;
    // center_calc[data[i].cluster_id].y_value += data[i].y_value;

    for (j = 0; j < 16; j++) {
      printf("Processing data[%d], cluster_id = %d, value[%d] = %f\n", i, data[i].cluster_id, j, data[i].values[j]);
      center_calc[data[i].cluster_id].values[j] += data[i].values[j];
    }
    data_size_per_cluster[data[i].cluster_id]++;
  }
  //计算每个cluster内点的X和Y的均值作为center
  for( i = 0; i < cluster_count; i++ )
  {
    if(data_size_per_cluster[i] != 0) {
      // center_calc[i].x_value = center_calc[i].x_value/ (double)(data_size_per_cluster[i]);
      // center_calc[i].y_value = center_calc[i].y_value/ (double)(data_size_per_cluster[i]);
      for (j = 0; j < 16; j++) {
        center_calc[i].values[j] /= (double)(data_size_per_cluster[i]);
      }
      printf(" cluster %d point cnt:%d\n", i, data_size_per_cluster[i]);
      //printf(" cluster %d center: X:%f, Y:%f\n", i, center_calc[i].x_value, center_calc[i].y_value);
      printf("cluster %d center: ", i);
      for (j = 0; j < 16; j++) {
        printf("%f ", center_calc[i].values[j]);
      }
      printf("\n");
    }
    else
      printf(" cluster %d count is zero\n", i);
  }
 
  //比较新的和旧的cluster center值的差别。如果是相等的，则停止K-means算法。
  compareNewOldClusterCenter(center_calc);
 
  //将新的cluster center的值放入cluster_center结构体中
  for( i = 0; i < cluster_count; i++ )
  {
    // cluster_center[i].x_value = center_calc[i].x_value;
    // cluster_center[i].y_value = center_calc[i].y_value;
    for (j = 0; j < 16; j++) {
      cluster_center[i].values[j] = center_calc[i].values[j];
    }
    cluster_center[i].cluster_id = i;
  }

  //在重新计算了新的cluster center之后，要重新来为每一个Point进行聚类，所以data中用于表示聚类ID的cluster_id要都重新置为-1。
  for( i = 0; i < data_size_total; i++ )
  {
    data[i].cluster_id = -1;
  }
}
 
//比较新旧的cluster center的值,完全一样表示聚类完成
void compareNewOldClusterCenter(CenterCalc* center_calc)
{
  int i, j;
  is_continue = 0;       //等于0表示不要继续，1表示要继续
  // for( i = 0; i < cluster_count; i++ )
  // {
  //   if( center_calc[i].x_value != cluster_center[i].x_value || center_calc[i].y_value != cluster_center[i].y_value)
  //   {
  //     is_continue = 1;
  //     break;
  //   }
  // }
  for (i = 0; i < cluster_count; i++){
    for (j = 0; j < 16; j++){
      if (center_calc[i].values[j] != cluster_center[i].values[j]){
        is_continue = 1;
        break;
      }
    }
    if (is_continue == 1)
      break;
  }
}
 
//K-means算法
void kmeans()
{
  int rounds;
  for( rounds = 0; rounds < MAX_ROUNDS; rounds++ )
  {
    printf("\nRounds : %d             \n", rounds+1);
    partition4AllPointOneCluster();
    calcClusterCenter();
    if( 0 == is_continue )
    {
       printf("\n after %d rounds, the classification is ok and can stop.\n", rounds+1);
       break;  
    }
  }
}