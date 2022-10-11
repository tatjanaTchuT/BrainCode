Get_Number_of_Populations <- function(folder_name){
  
  filename = list.files(path = folder_name,pattern="*_Data.dat");
  print(filename)
  full_filename <- paste(folder_name,"/",filename,sep="");
  print(full_filename);
  data  <- read.table(full_filename, header = TRUE, sep = "\t")
  data <- data[, colSums(is.na(data)) != nrow(data)]
  
  number_populations <- -3 + sqrt(8+ncol(data));#because ncol=1+6P+p^2
  print(number_populations);
  if(number_populations%%1!=0){
    stop("Number of columns in data file not following convention");
  }
  return(number_populations);
}

Load_data <- function(folder_name,number_populations) {
  
  filename = list.files(path = folder_name,pattern="*_Data.dat");
  full_filename <- paste(folder_name,"/",filename,sep="");
  
  data  <- read.table(full_filename, header = TRUE, sep = "\t")
  data <- data[1:(length(data)-1)]
  return(data);
}

# 
# PostProcess_data <- function(data,number_populations){
#   
#   var_names = c("V","r","mu_ext","mu","quenched","temporal");
#   df = data.frame();
#   
#   for(var_name_ in var_names){
#     
#     cols = c("time");
#     for(i in 0:(number_populations-1)){
#       cols <- append(cols,paste(var_name_,"_",toString(i),sep=""))
#     }
#     
#     sub_data <- data %>% select(cols) %>% gather(- time, key = "population", value = "value")
#     if(ncol(df) == 0){
#       df <- data.frame(sub_data);
#     }else{
#       df[var_name_] = sub_data["value"];
#     }
#     names(df)[names(df)=="value"] <- var_name_
#   }
# 
#   for(i in 0:(number_populations-1)){
#     df$population[df$population == paste(var_names[1],"_",toString(i),sep="")] <- i
#   }
#   df$population <- as.factor(df$population);
#   
#   return(df);
# }


Load_Synapses <- function(folder_name,number_populations) {
  filename = list.files(path = folder_name,pattern="*_Synapses.dat");
  full_filename <- paste(folder_name,"/",filename,sep="");
  
  if(file.exists(full_filename) && !dir.exists(full_filename)  ){
    data      <- read.table(full_filename, header = TRUE, sep = "\t")
    data <- data[1:(length(data)-1)]

  }else{

    return()

  }
  
  return(data);
}

Load_Potential <- function(folder_name,number_populations) {
  
  filename = list.files(path = folder_name,pattern="*_Potential.dat");
  full_filename <- paste(folder_name,"/",filename,sep="");

  if(file.exists(full_filename) && !dir.exists(full_filename)  ){
    data      <- read.table(full_filename, header = TRUE, sep = "\t")
    data <- data[1:(length(data)-1)]

  }else{
    return()

  }
  
  return(data);
}


Load_Currents <- function(folder_name,number_populations) {
  
  filename = list.files(path = folder_name,pattern="*_Currents.dat");
  full_filename <- paste(folder_name,"/",filename,sep="");

  
  if(file.exists(full_filename) && !dir.exists(full_filename)  ){
    data      <- read.table(full_filename, header = TRUE, sep = "\t")
    data <- data[1:(length(data)-1)]

  }else{
    return()

  }
  return(data);
}

Load_Rasterplot <- function(folder_name,number_populations) {
  
  filename = list.files(path = folder_name,pattern="*_Rasterplot.dat");
  full_filename <- paste(folder_name,"/",filename,sep="");

  if(file.exists(full_filename) && !dir.exists(full_filename)  ){
    data_rasterplot <- read.table(full_filename, header = TRUE, sep = "\t")
  }else{
    return()
  }
  return(data_rasterplot);
}


Load_Connectivity <-function(folder_name,postpop,prepop, bool_remove_last_row){
  Pattern <-paste("*_Connectivity_Matrix","_",toString(postpop),"_",toString(prepop),".txt",sep="")
  filename = list.files(path = folder_name,pattern=Pattern);
  full_filename <- paste(folder_name,"/",filename,sep="");
  if(file.exists(full_filename) && !dir.exists(full_filename)  ){
    data_ConnectivityMatrix <- read.table(full_filename, header = FALSE, sep = " ")
    if (bool_remove_last_row) {
      data_ConnectivityMatrix <- data_ConnectivityMatrix[1:(length(data_ConnectivityMatrix)-1)]
    }
  }else{
    return()
  }
  return(data_ConnectivityMatrix);
}

matequal <- function(x, y) {
  return(is.matrix(x) && is.matrix(y) && dim(x) == dim(y) && all(x == y));
}

Load_SynapsesConnection <-function(base_path,project_name,postpop,prepop){
  full_filename <-paste(dirname(dirname(base_path)),"/Input/",project_name,"_synapses_connection","_",toString(postpop),toString(prepop),".txt",sep="")
  
  if(file.exists(full_filename) && !dir.exists(full_filename)  ){
    data_SynapsesConnection <- read.table(full_filename, header = FALSE, sep = " ")
    #data_SynapsesConnection <- data_SynapsesConnection[1:(length(data_SynapsesConnection)-1)]
  }else{
    return()
  }
  return(data_SynapsesConnection);
}

Load_Delay <-function(folder_name,postpop,prepop){
  Pattern <-paste("*_DelayConnectivity_Matrix","_",toString(postpop),"_",toString(prepop),".txt",sep="")
  filename = list.files(path = folder_name,pattern=Pattern);
  full_filename <- paste(folder_name,"/",filename,sep="");
  if(file.exists(full_filename) && !dir.exists(full_filename)  ){
    data_DelayMatrix <- read.table(full_filename, header = FALSE, sep = " ")
    data_DelayMatrix <- data_DelayMatrix[1:(length(data_DelayMatrix)-1)]
  }else{
    return()
  }
  return(data_DelayMatrix);
}

Load_Jpot <-function(folder_name,postpop,prepop){
  Pattern <-paste("*_JPotConnectivity_Matrix","_",toString(postpop),"_",toString(prepop),".txt",sep="")
  filename = list.files(path = folder_name,pattern=Pattern);
  full_filename <- paste(folder_name,"/",filename,sep="");
  if(file.exists(full_filename) && !dir.exists(full_filename)  ){
    data_JpotMatrix <- read.table(full_filename, header = FALSE, sep = " ")
    data_JpotMatrix <- data_JpotMatrix[1:(length(data_JpotMatrix)-1)]
  }else{
    return()
  }
  return(data_JpotMatrix);
}
