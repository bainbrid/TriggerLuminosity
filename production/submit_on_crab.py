from CRABClient.UserUtilities import config, ClientException, getUsernameFromCRIC
import yaml
import datetime
from fnmatch import fnmatch
from argparse import ArgumentParser

production_tag = datetime.date.today().strftime('%Y%b%d')

config = config()
config.section_('General')
config.General.transferOutputs = True
config.General.transferLogs = True
config.General.workArea = 'TriggerLuminosity_{:s}'.format(production_tag)

config.section_('Data')
config.Data.publication = False
config.Data.outLFNDirBase = '/store/group/phys_bphys/DiElectronX/test/trigger/{:s}'.format(config.General.workArea)
config.Data.inputDBS = 'global'

config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = '../test/test_cfg.py'
#config.JobType.maxJobRuntimeMin = 3000
#config.JobType.allowUndistributedCMSSW = True

config.section_('User')
config.section_('Site')
config.Site.storageSite = 'T2_CH_CERN'

if __name__ == '__main__':

  from CRABAPI.RawCommand import crabCommand
  from CRABClient.ClientExceptions import ClientException
  from http.client import HTTPException
  from multiprocessing import Process

  def submit(config):
      try:
          crabCommand('submit', config = config)
      except HTTPException as hte:
          print("Failed submitting task:",hte.headers)
      except ClientException as cle:
          print("Failed submitting task:",cle)

  parser = ArgumentParser()
  parser.add_argument('-y', '--yaml', default = 'samples_Run3.yml', help = 'File with dataset descriptions')
  parser.add_argument('-f', '--filter', default='*', help = 'filter samples, POSIX regular expressions allowed')
  parser.add_argument('-r', '--lhcRun', type=int, default=3, help = 'Run 2 or 3 (default)')
  args = parser.parse_args()

  with open(args.yaml) as f:
    doc = yaml.load(f,Loader=yaml.FullLoader) # Parse YAML file
    common = doc['common'] if 'common' in doc else {'data' : {}, 'mc' : {}}
    
    # loop over samples
    for sample, info in doc['samples'].items():
      # Input DBS
      input_dbs = info['dbs'] if 'dbs' in info else None
      # Given we have repeated datasets check for different parts
      parts = info['parts'] if 'parts' in info else [None]
      for part in parts:
        name = sample.replace('%d',str(part)) if part is not None else sample

        # filter names according to what we need
        if not fnmatch(name, args.filter): continue
        print('################'+'#'*len(name)+'####################################')
        print('### Submitting \"'+name+'\" with the following CRAB config ###')
        print('################'+'#'*len(name)+'####################################')

        isMC = info['isMC']

        config.Data.inputDBS = input_dbs if input_dbs is not None else 'global'

        config.Data.inputDataset = info['dataset'].replace('%d',str(part)) \
                                   if part is not None else \
                                      info['dataset']

        config.General.requestName = name
        common_branch = 'mc' if isMC else 'data'
        config.Data.splitting = 'FileBased' if isMC else 'LumiBased'
        if not isMC:
            config.Data.lumiMask = info.get(
                'lumimask', 
                common[common_branch].get('lumimask', None)
            )
        else:
            config.Data.lumiMask = ''

        config.Data.unitsPerJob = info.get(
            'splitting',
            common[common_branch].get('splitting', None)
        )
        globaltag = info.get(
            'globaltag',
            common[common_branch].get('globaltag', None)
        )
        
        config.JobType.pyCfgParams = [
            'isMC={:.0f}'.format(int(isMC)),
            'reportEvery=1000',
            'tag={:s}'.format(production_tag),
            'globalTag={:s}'.format(globaltag),
            'lhcRun={:.0f}'.format(args.lhcRun),
        ]
        
        config.JobType.outputFiles = ['output.json']

        print(config)
        submit(config)
