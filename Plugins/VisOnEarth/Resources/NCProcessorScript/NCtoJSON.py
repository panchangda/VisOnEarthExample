import numpy as np
import json
from netCDF4 import Dataset


class DataProcess:
    def __init__(self):
        self.data = {}

    def load_netCDF(self, file_path):
        nc = Dataset(file_path, 'r')
        self.data = {}

        # 解析维度
        self.data['dimensions'] = {
            'lon': nc.dimensions['lon'].size,
            'lat': nc.dimensions['lat'].size,
            'lev': nc.dimensions['lev'].size
        }
        # todo: Add Field: lonRange, latRange, WMax, WMin 
        self.data['property'] = {
            'Umax':nc.variables['U'].max,
            'Umin':nc.variables['U'].min,
            'Vmax':nc.variables['V'].max,
            'Vmin':nc.variables['V'].min,
        }

        # 解析变量
        # todo: Add Field: W and its array
        self.data['lon'] = {
            'array' : np.array(nc.variables['lon'][:]).flatten()
        }
        
        self.data['lat'] = {
             'array' : np.array(nc.variables['lat'][:]).flatten(),
        }
        
        self.data['lev'] = {
             'array' : np.array(nc.variables['lev'][:]).flatten(),
        }
        self.data['U'] = {
             'array' : np.array(nc.variables['U'][:]).flatten(),
        }
        self.data['V'] = {
             'array' : np.array(nc.variables['V'][:]).flatten(),
        }
        
        # 解析变量属性
        # for var_name in ['U', 'V']:
        #     var_attributes = nc.variables[var_name].ncattrs()
        #     self.data[var_name].update({
        #         attr: nc.variables[var_name].getncattr(attr) for attr in var_attributes
        #     })

        nc.close()

    def load_data(self, nc_file_path):
        self.load_netCDF(nc_file_path)
        return self.data

    def randomize_particles(self, max_particles, viewer_parameters):
        particles = np.zeros((max_particles, 4), dtype=np.float32)
        particles[:, 0] = np.random.uniform(viewer_parameters['lonRange'][0], viewer_parameters['lonRange'][1], max_particles)
        particles[:, 1] = np.random.uniform(viewer_parameters['latRange'][0], viewer_parameters['latRange'][1], max_particles)
        particles[:, 2] = np.random.uniform(self.data['lev']['min'], self.data['lev']['max'], max_particles)
        particles[:, 3] = 0.0
        return particles
    
    def save_to_json(self, file_path):
        def default(obj):
            if isinstance(obj, np.ndarray):
                return obj.tolist()  # 将Numpy数组转换为列表
            elif hasattr(obj, 'tolist'):  # 检查是否可以直接转换为列表
                return obj.tolist()
            elif isinstance(obj, np.generic):  # 检查是否为Numpy数据类型
                return np.asscalar(obj)
            else:
                ype_name = obj.__class__.__name__
                raise TypeError(f"Object of type '{type_name}' is not JSON serializable")

        with open(file_path, 'w') as f:
            json.dump(self.data, f, default=default, indent=4)


# 示例使用
# nc_file_path: 输入的NC文件位置
# json_file_path： 输出的JSON文件位置
nc_file_path = "E:\\Codes\\netcdf\\data\\demo.nc"
json_file_path = "E:\\Codes\\netcdf\\data\\demo.json"
data_processor = DataProcess()
data_processor.load_data(nc_file_path)
data_processor.save_to_json(json_file_path)